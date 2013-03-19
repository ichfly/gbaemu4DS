#include <nds.h>
#include <stdio.h>

#define KEY_PARAM CARD_SEC_EN | CARD_SEC_DAT | CARD_SEC_CMD | CARD_CLK_SLOW

void swiDelay(int i)
{
	for(vu32 t = 0; t < i;i++);
}

#ifdef CARD_DEBUG
static inline int dbg_printf( const char* format, ... )
{
    va_list args;
    va_start( args, format );
    int ret = vprintf( format, args );
    va_end(args);
    return ret;
}
#else
#define dbg_printf(...)
#endif//DEBUG

static u16 SDCardAddr=0;
static bool _isSD20=false;
static bool _isSDHC=false;
static u32 _size=0;


#define PAGESIZE_0    (0<<24)
#define PAGESIZE_4    (7<<24)
#define PAGESIZE_512  (1<<24)
#define PAGESIZE_1K   (2<<24)
#define PAGESIZE_2K   (3<<24)
#define PAGESIZE_4K   (4<<24)

static u32 lastCmd[2];
void ioRpgSendCommand(u32 command[2],u32 pageSize,u32 latency,void* buffer)
{
  REG_AUXSPICNTH=CARD_CR1_ENABLE|CARD_CR1_IRQ;

  for(u32 i=0;i<2;++i)
  {
    CARD_COMMAND[i*4+0]=command[i]>>24;
    CARD_COMMAND[i*4+1]=command[i]>>16;
    CARD_COMMAND[i*4+2]=command[i]>>8;
    CARD_COMMAND[i*4+3]=command[i]>>0;
  }

  pageSize-=pageSize&3; // align to 4 byte
  u32 pageParam=PAGESIZE_4K;
  u32 transferLength=4096;
  // make zero read and 4 byte read a little special for timing optimization(and 512 too)
  if(0==pageSize)
  {
    transferLength=0;
    pageParam=PAGESIZE_0;
  }
  else if(4==pageSize)
  {
    transferLength=4;
    pageParam=PAGESIZE_4;
  }
  else if(512==pageSize)
  {
    transferLength=512;
    pageParam=PAGESIZE_512;
  }

  // go
  REG_ROMCTRL=0;
  REG_ROMCTRL=KEY_PARAM|CARD_ACTIVATE|CARD_nRESET|pageParam|latency;

  u8* pbuf=(u8*)buffer;
  u32* pbuf32=(u32*)buffer;
  bool useBuf=(NULL!=pbuf);
  bool useBuf32=(useBuf&&(0==(3&((unsigned int)buffer))));

  u32 count=0;
  u32 cardCtrl=REG_ROMCTRL;
  while((cardCtrl&CARD_BUSY)&&count<pageSize)
  {
    cardCtrl=REG_ROMCTRL;
    if(cardCtrl&CARD_DATA_READY)
    {
      u32 data=CARD_DATA_RD;
      if(useBuf32&&count<pageSize)
      {
        *pbuf32++=data;
      }
      else if(useBuf&&count<pageSize)
      {
        pbuf[0]=(unsigned char)(data>> 0);
        pbuf[1]=(unsigned char)(data>> 8);
        pbuf[2]=(unsigned char)(data>>16);
        pbuf[3]=(unsigned char)(data>>24);
        pbuf+=sizeof(unsigned int);
      }
      count+=4;
    }
  }

  // if read is not finished, ds will not pull ROM CS to high, we pull it high manually
  if(count!=transferLength)
  {
    // MUST wait for next data ready,
    // if ds pull ROM CS to high during 4 byte data transfer, something will mess up
    // so we have to wait next data ready
    do { cardCtrl=REG_ROMCTRL; } while(!(cardCtrl&CARD_DATA_READY));
    // and this tiny delay is necessary
    swiDelay(33);
    // pull ROM CS high
    REG_ROMCTRL=0;
    REG_ROMCTRL=KEY_PARAM|CARD_ACTIVATE|CARD_nRESET|0|0x0000;
  }
  // wait rom cs high
  do { cardCtrl=REG_ROMCTRL; } while(cardCtrl&CARD_BUSY);
  lastCmd[0]=command[0];lastCmd[1]=command[1];
}

bool ioRpgWaitCmdBusy(bool forceWait)
{
  bool timeout=true;
  while( timeout && forceWait )
  {
    REG_AUXSPICNTH=CARD_CR1_ENABLE|CARD_CR1_IRQ;
    for(u32 i=0;i<8;++i) CARD_COMMAND[i]=0xB8;
    // go
    REG_ROMCTRL=0;
    REG_ROMCTRL=KEY_PARAM|CARD_ACTIVATE|CARD_nRESET|(6<<24)|0;

    u32 count=0;
    u32 transferLength=256<<6;
    u32 data=0;
    u32 cardCtrl=REG_ROMCTRL;

    while(cardCtrl&CARD_BUSY)
    {
      cardCtrl=REG_ROMCTRL;
      if(cardCtrl&CARD_DATA_READY)
      {
        data=CARD_DATA_RD;
        count+=4;
        if(0x00000fc2==data)
        {
          timeout=false;
          break;
        }
      }
      swiDelay( 16 ); // 1us
    }

    if(count!= transferLength) // if read is not finished, ds will not pull ROM CS to high, we pull it high manually
    {
      do { cardCtrl=REG_ROMCTRL; } while(!(cardCtrl&CARD_DATA_READY));
      swiDelay(33);
      REG_ROMCTRL=0;
      REG_ROMCTRL=KEY_PARAM|CARD_ACTIVATE|CARD_nRESET|0|0x0000;
    }

    // wait rom cs high
    do { cardCtrl=REG_ROMCTRL; } while(cardCtrl&CARD_BUSY);

    if(timeout)
    {
      dbg_printf("ioRpgWaitCmdBusy time out, ");
      dbg_printf("(%08x) lastCmd %08x %08x\n",data,lastCmd[0],lastCmd[1]);
    }
  }
  return !timeout;
}




static void composeResponse1(void* data,void* response)
{
  const u8* pdata=(u8*)data;
  u8* pResponse=(u8*)response;
  *(u32*)pResponse=0;

  u32 bitCount=39;
  for(int i=0;i<4;++i)
  {
    for(int j=0;j<8;++j)
    {
      u8 bit=(pdata[bitCount--]&0x80)?1:0;
      pResponse[i]|=bit<<j;
    }
  }
}

static void composeResponse2(void* data,void* response)
{
  const u8* pdata=(u8*)data;
  u8* pResponse=(u8*)response;
  *(u64*)response=*(((u64 *)response)+1)=0;

  u32 bitCount=135;
  for(int i=0;i<16;++i)
  {
    for(int j=0;j<8;++j)
    {
      u8 bit=(pdata[bitCount--]&0x80)?1:0;
      pResponse[i]|=bit<<j;
    }
  }
}

static u32 calcSDSize(u8* data)
{
  u32 result=0;
  u32 type=data[15]>>6;
  switch(type)
  {
    case 0:
      {
        u32 block_len=data[10]&0xf;
        block_len=1<<block_len;
        u32 mult=(data[5]>>7)|((data[6]&3)<<1);
        mult=1<<(mult+2);
        result=data[9]&3;
        result=(result<<8)|data[8];
        result=(result<<2)|(data[7]>>6);
        result=(result+1)*mult*block_len/512;
      }
      break;
    case 1:
      result=data[8]&0x3f;
      result=(result<<8)|data[7];
      result=(result<<8)|data[6];
      result=(result+1)*1024;
      break;
  }
  return result;
}

static void sendSDCommandR0(u32* cmd)
{
  ioRpgSendCommand(cmd,0,80,NULL);
}

static void sendSDCommandR1(u32* cmd,u8* response)
{
  ALIGN(4) u8 tempResponse[64];
  tempResponse[0]=0;
  ioRpgSendCommand(cmd,48,40,tempResponse+1);
  composeResponse1(tempResponse,response);
}

static void sendSDCommandR2(u32* cmd,u8* response)
{
  ALIGN(4) u8 tempResponse[160];
  tempResponse[0]=0;
  ioRpgSendCommand(cmd,136,40,tempResponse+1);
  composeResponse2(tempResponse,response);
}


bool sddInitSD(void)
{
  // reset sd card
  {
    u32 cmd[2]={0xab000000,0x00000000};
    sendSDCommandR0(cmd);
  }

  // CMD8 for SDHC initialization
  {
    ALIGN(4) u8 response[4];
    u32 cmd[2]={0xab010008,0x000001AA};
    sendSDCommandR1(cmd,response);
    _isSD20=(*(u32*)response==cmd[1]);
    dbg_printf("cmd %d response %08x\n",0x08,*(u32*)response);
  }

  u32 retry=0;

  while(retry < 10000) // about 1 second
  {
    // ACMD41
    {
      ALIGN(4) u8 response[4];
      u32 cmd[2]={0xab010037,0x00000000};
      sendSDCommandR1(cmd,response);
      //dbg_printf("cmd %d response %08x\n",0x37,*(u32*)response);
    }

    {
      ALIGN(4) u8 response[4];
      u32 cmd[2]={0xab010029,0x00ff8000};
      if(_isSD20) cmd[1]|=0x40000000;
      sendSDCommandR1(cmd,response);
      //dbg_printf("cmd %d response %08x\n",0x29,*(u32*)response);
      if(!(response[3]&0x80))
      {
        swiDelay(1666);
        retry++;
      }
      else
      {
        _isSDHC=(*(u32*)response&0x40000000);
        break;
      }
    }
  }
  dbg_printf("retry %d times\n",retry);

  // CMD2
  {
    ALIGN(4) u8 response[16];
    u32 cmd[2]={0xab010002,0x00000000};
    sendSDCommandR2(cmd,response);
    dbg_printf("cmd 02 response %08x\n",*(u32*)response);
  }

  // CMD3
  {
    ALIGN(4) u8 response[4];
    u32 cmd[2]={0xab010003,0x00000000};
    sendSDCommandR1(cmd,response);
    dbg_printf("cmd 03 response %08x\n",*(u32*)response);
    SDCardAddr=*(u32*)response>>16;
  }

  // CMD9
  {
    ALIGN(4) u8 response[16];
    u32 cmd[2] = { 0xab010009, 0x00000000 | (SDCardAddr << 16) };
    sendSDCommandR2( cmd, response );
    dbg_printf("cmd 09 response\n%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x \n",response[0],response[1],response[2],response[3],response[4],response[5],response[6],response[7],response[8],response[9],response[10],response[11],response[12],response[13],response[14],response[15]);
    _size=calcSDSize(response);
  }

  // CMD7
  {
    ALIGN(4) u8 response[4];
    u32 cmd[2]={0xab010007,0x00000000|(SDCardAddr<<16)};
    sendSDCommandR1(cmd,response);
    dbg_printf("cmd 07 response %08x\n",*(u32*)response);
  }

  // ACMD6
  {
    ALIGN(4) u8 response[4];
    u32 cmd[2]={0xab010037,0x00000000|(SDCardAddr<<16)};
    sendSDCommandR1(cmd,response);
    dbg_printf("cmd %d response %08x\n",0x37,*(u32*)response);
  }

  {
    ALIGN(4) u8 response[4];
    u32 cmd[2]={0xab010006,0x00000002};
    sendSDCommandR1(cmd,response);
    dbg_printf("cmd 06 response %08x\n",*(u32*)response);
  }

  // CMD13
  {
    ALIGN(4) u8 response[4];
    u32 cmd[2]={0xab01000D,0x00000000|(SDCardAddr<<16)};
    sendSDCommandR1(cmd,response);
    u8 state=(response[1]>>1); // response bit 9 - 12
    if(4==state)
    {
      dbg_printf("sd init ok. SD2.0=%d,SDHC=%d\n",_isSD20,_isSDHC);
      return true;
    }
    else
    {
      dbg_printf("sd init fail\n");
      _isSDHC=false;
      _isSD20=false;
      return false;
    }

    dbg_printf(" the last response is\n%08x\n",*(u32*)response);
  }
}


void sddReadBlocks(u32 addr,void* buffer)
{
  u8* pBuffer=(u8*)buffer;

  u32 address=_isSDHC?addr:(addr<<9);
  u32 sdReadSingleBlock[2]={0xab230011,address}; // input read address here
  sendSDCommandR0(sdReadSingleBlock);
  ioRpgWaitCmdBusy(true);

  u32 readSD[2]={0xB7000000,0x00000000|0x00130000}; // address dont care here
  ioRpgSendCommand(readSD,512,4,pBuffer);
  swiDelay(16*5); // wait for sd crc auto complete
 
}
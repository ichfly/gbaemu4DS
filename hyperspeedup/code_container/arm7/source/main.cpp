		//sound alloc
		//0-3 matching gba
		//4-5 FIFO
		//ledBlink(1);
		//swiWaitForVBlank();



int sock = 0;                        /* Socket descriptor */

#include <nds.h>
#include <nds/arm7/audio.h>

#include "main.h"

#include "dsregs.h"

#include "timer20.h"

#include "../../gloabal/cpuglobal.h"

#include "wifi_arm7.h"

#include <netinet/in.h>

void installWifiFIFO();

#ifdef anyarmcom
u32* amr7sendcom = 0;
u32* amr7senddma1 = 0;
u32* amr7senddma2 = 0;
u32* amr7recmuell = 0;
u32* amr7directrec = 0;
u32* amr7indirectrec = 0;
u32* amr7fehlerfeld;
u32 currfehler = 0;
#define maxfehler 8
#endif

bool __dsimode = false;

//#define checkforerror

//timerStart timeval sucks need a new idear to handel that
//also we are 3 sampels + some overlayer back in the file fix that


//internal vals

u16 callline = 0xFFFF;

bool autodetectdetect = false;

u8 tacktgeber_sound_FIFO_DMA_A = 0;
u8 tacktgeber_sound_FIFO_DMA_B = 0;

u8* soundbuffA = 0;
u8* soundbuffB = 0;

u8 dmaApart = 0;
u8 dmaBpart = 0;

u32 dmabuffer = 0;

//sound regs

u16 SOUNDCNT_L = 0;
u16 SOUNDCNT_H = 0;

u16 TM0CNT_L = 0;
u16 TM1CNT_L = 0;

u16 TM0CNT_H = 0;
u16 TM1CNT_H = 0;

u16 DMA1CNT_H = 0;
u16 DMA2CNT_H = 0;


u16 DMA1SAD_L = 0;
u16 DMA1SAD_H  = 0;
u16 DMA1DAD_L = 0;
u16 DMA1DAD_H  = 0;

u16 DMA2SAD_L = 0;
u16 DMA2SAD_H  = 0;
u16 DMA2DAD_L = 0;
u16 DMA2DAD_H  = 0;

u16 SOUND3CNT_H = 0;
u16 SOUND4CNT_H = 0;
u16 SOUND4CNT_L = 0;
u16 SOUND1CNT_L = 0;
u16 SOUND1CNT_H = 0;
u16 SOUND1CNT_X = 0;
u16 SOUND2CNT_H = 0;
u16 SOUND2CNT_X = 0;
u16 SOUND3CNT_L = 0;
u16 SOUND3CNT_X = 0;


//u8 WAVE_RAM[0x10*3*2];
u8* WAVE_RAM;

//more or less debug stuff

vu32 debugsrc1 = 0;
vu32 debugsrc2  = 0;
vu32 debugfr1 = 0;
vu32 debugfr2  = 0;

//DSP sound data pointer
u32 pointer_7Bitpoly;
u32 pointer_15Bitpoly;

//debug stuff
void senddebug32(u32 val)
{
	REG_IPC_FIFO_TX = 0x4000BEEF;
	REG_IPC_FIFO_TX = val;
#ifdef anyarmcom
	*amr7sendcom = *amr7sendcom + 2;
#endif
}

void sendwififail32(u32 val)
{
	REG_IPC_FIFO_TX = 0x4400BEEF;
	REG_IPC_FIFO_TX = val;
#ifdef anyarmcom
	*amr7sendcom = *amr7sendcom + 2;
#endif
}

bool ykeypp =false;
//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	Wifi_Update();
	if(!(REG_KEYXY & 0x2)) //can't do that by irq
	{
		if(!ykeypp)
		{
			REG_IPC_FIFO_TX = 0x4200BEEF;
#ifdef anyarmcom
			*amr7sendcom = *amr7sendcom + 1;
#endif
			ykeypp = true;

		}
	}
	else
	{
		ykeypp = false;
	}
	if(*(u16*)0x04000136 & 0x80) //close nds
	{
		closelib();
	}
}

//time based functions
u32 stop(u8 back)
{
	if(back < 2)SCHANNEL_CR(back + 8) &= ~0x80000000;
	else SCHANNEL_CR(back) &= ~0x80000000;
	return 0;
}

u32 sweepspeed = 0;
u32 sweep(u8 back)
{
	if(SOUND1CNT_L & 0x8)sweepspeed = sweepspeed - sweepspeed/(1<<(SOUND1CNT_L & 0x7));
	else sweepspeed = sweepspeed + sweepspeed/(1<<(SOUND1CNT_L & 0x7));
	SCHANNEL_TIMER(8) = -(33513982/2)/(sweepspeed);
	return TIMER_FREQ(0x80/(((SOUND1CNT_L & 0x70) >> 4) + 1));
}
u32 Envelope(u8 back)
{
	u32 Vol = SOUNDCNT_H & 0x3;
	switch(Vol)
	{
	case 3:
	case 0:
		Vol = 1;
		break;
	case 1:
		Vol = 2;
		break;
	case 2:
		Vol = 4;
		break;
	}
	u32 Masterright = SOUNDCNT_L & 0x7;
	u32 Masterleft =  (SOUNDCNT_L >> 4) & 0x7;
	if(back == 1)
	{
		if(SOUND1CNT_H & 0x800)SOUND1CNT_H += 0x1000;
		else SOUND1CNT_H -= 0x1000;
		SCHANNEL_CR(8) = ((SCHANNEL_CR(8) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(8)) >> 8) + Masterleft * ((SOUNDCNT_L & BIT(12)) >> 12) ) * Vol) * ((SOUND1CNT_L & 0xF000) >> 12)*127/840);  //max:127
		return TIMER_FREQ(0x40/((SOUND1CNT_H & 0x700) >> 8));
	} else if (back == 2)
	{
		if(SOUND2CNT_H & 0x800)SOUND2CNT_H += 0x1000;
		else SOUND2CNT_H -= 0x1000;
		SCHANNEL_CR(9) = ((SCHANNEL_CR(9) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(9)) >> 9) + Masterleft * ((SOUNDCNT_L & BIT(13)) >> 13) ) * Vol) * ((SOUND2CNT_H & 0xF000) >> 12)*127/840);  //max:127
		return TIMER_FREQ(0x40/((SOUND2CNT_H & 0x700) >> 8));
	} else if(back == 3)
	{
		if(SOUND4CNT_L & 0x800)SOUND4CNT_L += 0x1000;
		else SOUND4CNT_L -= 0x1000;
		SCHANNEL_CR(3) = ((SCHANNEL_CR(3) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(11)) >> 11) + Masterleft * ((SOUNDCNT_L & BIT(15)) >> 15) )* Vol) * ((SOUND4CNT_L & 0xF000) >> 12)*127/840);
		return TIMER_FREQ(0x40/((SOUND4CNT_L & 0x700) >> 8));
	}
}

void dmaAtimerinter()
{
	if(dmaApart)REG_IPC_FIFO_TX = debugsrc1 + 0x400001;
	else REG_IPC_FIFO_TX = debugsrc1 + 0x400000;
	debugsrc1+=0x10;
	if(dmaApart == 0) dmaApart = 1;
	else dmaApart = 0;
#ifdef anyarmcom
	*amr7senddma1 = *amr7senddma1 + 1;
#endif
}
void dmaBtimerinter()
{
	if(dmaApart)REG_IPC_FIFO_TX = debugsrc1 + 0x400002;
	else REG_IPC_FIFO_TX = debugsrc1 + 0x400003;
	debugsrc2+=0x10;
	if(dmaBpart == 0) dmaBpart = 1;
	else dmaBpart = 0;
#ifdef anyarmcom
	*amr7senddma2 = *amr7senddma2 + 1;
#endif
}



void newvalwrite8(u32 addr,u8 val)
{
			switch(addr)
			{

			  case 0x90:
			  case 0x91:
			  case 0x92:
			  case 0x93:
			  case 0x94:
			  case 0x95:
			  case 0x96:
			  case 0x97:
			  case 0x98:
			  case 0x99:
			  case 0x9a:
			  case 0x9b:
			  case 0x9c:
			  case 0x9d:
			  case 0x9e:
			  case 0x9f:
				{
					  u16 valtostr =  ((u8)((((val &0xF0) << 0) + ((val &0xF0) >> 4)) - 0x80) << 0);
					  valtostr |=  ((u8)((((val &0xF) << 0) + ((val &0xF) << 4)) - 0x80) << 8);
					  if(val&0x40)
					  {
							*(u16*)&WAVE_RAM[(addr - 0x90)*2] = valtostr;
							*(u16*)&WAVE_RAM[(addr - 0x70)*2] = valtostr;
					  }
					  else
					  {
							*(u16*)&WAVE_RAM[(addr - 0x80)*2] = valtostr;
					  }
				  }
				break;
				default:
#ifdef anyarmcom
				*amr7recmuell = *amr7recmuell + 1;
				amr7fehlerfeld[currfehler] = addr;
				amr7fehlerfeld[currfehler + 1] = val;
				currfehler+= 2;
				if(currfehler == maxfehler)currfehler = 0;
#endif
				break;
			}
}

void newvalwrite16(u32 addr,u32 val)
{

			switch(addr)
			{
			case 0x60:
				SOUND1CNT_L = val;
				break;
			case 0x62:
				{
					SCHANNEL_CR(8) &= ~0x80000000;
					SOUND1CNT_H = val;
					switch(val & 0xC0)
					{
					case 0:
						SCHANNEL_CR(8) = (SCHANNEL_CR(8) & ~0x07000000) | 0x60000200; //start div by 4 and PSG (12,5% up)
						break;
					case 0x40:
						SCHANNEL_CR(8) = (SCHANNEL_CR(8) & ~0x07000000) | 0x61000200; //start div by 4 and PSG (25% up)
						break;
					case 0x80:
						SCHANNEL_CR(8) = (SCHANNEL_CR(8) & ~0x07000000) | 0x63000200; //start div by 4 and PSG (50% up)
						break;
					case 0xC0:
						SCHANNEL_CR(8) = (SCHANNEL_CR(8) & ~0x07000000) | 0x65000200; //start div by 4 and PSG (75% up)
						break;
					}
					SCHANNEL_CR(8) |= 0x80000000;
				}
			break;
			case 0x64:
				SOUND1CNT_X = val;
				SCHANNEL_TIMER(8) = -(33513982/2)/(131072*8/(2048-(val&0x7FF)));
				if(val &0x8000) //reinit
				{
					SCHANNEL_CR(8) &= ~0x80000000;
					sweepspeed = 131072*8/(2048-(val&0x7FF));
					if((SOUND1CNT_L & 0x7) != 0)timerotheradd(0,TIMER_FREQ(0x80/(((SOUND1CNT_L & 0x70) >> 4) + 1)),stop);
					else timerotheradd(0,0,stop);
					if((SOUND1CNT_H & 0x700) != 0)
					{
						timerotheradd(1,TIMER_FREQ(0x40/((SOUND1CNT_H & 0x700) >> 8)),Envelope);
					}
					else
					{
						timerotheradd(1,0,Envelope);
					}
					updatevol(); //todo do it better
					SCHANNEL_CR(8) |= 0x80000000;

				}
				if(val &0x4000)
				{
					timerlenadd(0,TIMER_FREQ(256/(0x40-(SOUND1CNT_H&0x3F))),stop);
				}
				break;
			case 0x68:
				{
					SCHANNEL_CR(9) &= ~0x80000000;
					SOUND2CNT_H = val;
					switch(val & 0xC0)
					{
					case 0:
						SCHANNEL_CR(9) = (SCHANNEL_CR(9) & ~0x07000000) | 0x60000200; //start div by 4 and PSG (12,5% up)
						break;
					case 0x40:
						SCHANNEL_CR(9) = (SCHANNEL_CR(9) & ~0x07000000) | 0x61000200; //start div by 4 and PSG (25% up)
						break;
					case 0x80:
						SCHANNEL_CR(9) = (SCHANNEL_CR(9) & ~0x07000000) | 0x63000200; //start div by 4 and PSG (50% up)
						break;
					case 0xC0:
						SCHANNEL_CR(9) = (SCHANNEL_CR(9) & ~0x07000000) | 0x65000200; //start div by 4 and PSG (75% up)
						break;
					}
					SCHANNEL_CR(9) |= 0x80000000;
				}
				break;
			case 0x6C:
				SOUND2CNT_X = val;
				SCHANNEL_TIMER(9) = -(33513982/2)/(131072*8/(2048-(val&0x7FF)));
				if(val &0x8000) //reinit
				{
					SCHANNEL_CR(9) &= ~0x80000000;
					if((SOUND2CNT_H & 0x700) != 0)
					{
						timerotheradd(2,TIMER_FREQ(0x40/((SOUND2CNT_H & 0x700) >> 8)),Envelope);
					}
					else
					{
						timerotheradd(2,0,Envelope);
					}
					updatevol(); //todo do it better
					SCHANNEL_CR(9) |= 0x80000000;
				}
				if(val &0x4000)
				{
					timerlenadd(1,TIMER_FREQ(256/(0x40-(SOUND2CNT_H&0x3F))),stop);
				}
				break;
			case 0x78:
				SOUND4CNT_L = val;
				break;
			case 0x7C:
				{
				int r = val & 0x7 << 1;
				int s = (val &0xF0) >> 4;
				if(r = 0)r = 1;
				SCHANNEL_TIMER(3) = -(33513982/2)/(524288 * 2 / r / (1<<(s+1)));
				if(val&0x8)
				{
					SCHANNEL_SOURCE(3) = pointer_7Bitpoly;
					SCHANNEL_LENGTH(3) = 0x7F;
				}
				else
				{
					SCHANNEL_SOURCE(3) = pointer_15Bitpoly;
					SCHANNEL_LENGTH(3) = 0x7FFF;
				}
				if(val &0x8000) //reinit
				{
					SCHANNEL_CR(3) &= ~0x80000200;
					SCHANNEL_CR(3) |= 0x80000200;
					if((SOUND4CNT_L & 0x700) != 0)
					{
						timerotheradd(3,TIMER_FREQ(0x40/((SOUND4CNT_L & 0x700) >> 8)),Envelope);
					}
					else
					{
						timerotheradd(3,0,Envelope);
					}
					updatevol(); //todo do it better
				}
				//SCHANNEL_CR(3) |= 0x80000200 | SOUND_REPEAT; //start div by 4
				SOUND4CNT_H = val;
				if(val &0x4000)
				{
					timerlenadd(3,TIMER_FREQ(256/(0x40-(SOUND4CNT_L&0x3F))),stop);
				}
				}
				break;
			case 0x72:
				{
					SOUND3CNT_H = val;

					u32 Vol = SOUNDCNT_H & 0x3;
					u32 Masterright = SOUNDCNT_L & 0x7;
					u32 Masterleft =  (SOUNDCNT_L >> 4) & 0x7;
					u16 sound3dif = (SOUND3CNT_H & 0x6000) >> 14;
					u16 sound3mul = 1;
					if (sound3dif = 0)
					{
						sound3dif = 1;
						sound3mul = 0;
					}
					if(SOUND3CNT_H & 0x8000)
					{
						sound3dif = 4;
						sound3mul = 3;
					}
					SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(10)) >> 10) + Masterleft * ((SOUNDCNT_L & BIT(14)) >> 14) ) * Vol * 2 * sound3mul / sound3dif );//max:112
				}
				break;
			case 0x70:
				SOUND3CNT_L = val;
				SCHANNEL_CR(2) &= ~0x80000000; //stop
				if(val&0x20) SCHANNEL_LENGTH(2) = 0x10*2*2;
				else SCHANNEL_LENGTH(2) = 0x10*2;
				if(val&0x40) SCHANNEL_SOURCE(2) = (u32)WAVE_RAM;
				else SCHANNEL_SOURCE(2) = (u32)WAVE_RAM +0x10*2;
				if(val&0x80)SCHANNEL_CR(2) |= 0x80000200 | SOUND_REPEAT; //start div by 4
				//SCHANNEL_CR(3) |= 0x80000200 | SOUND_REPEAT; //start div by 4
				break;
			case 0x74:
				SOUND3CNT_X = val;
				SCHANNEL_TIMER(2) = -(33513982/2)/(2097152/(2048-(val&0x7FF)));
				if(val &0x4000)
				{
					timerlenadd(2,TIMER_FREQ(256/(0x40-(SOUND3CNT_H&0x3F))),stop);
				}
				break;
			  case 0xBC:
				DMA1SAD_L = val;
				break;
			  case 0xBE:
				DMA1SAD_H = val & 0x0FFF;
				break;
			  case 0xC0:
				DMA1DAD_L = val;
				checkstart();
				break;
			  case 0xC2:
				DMA1DAD_H = val & 0x07FF;
				checkstart();
				break;
			  case 0xC4:
				//DMA1CNT_L = val & 0x3FFF;
				break;
			  case 0xC6:
				DMA1CNT_H = val;
				checkstart();
				break;
			  case 0xC8:
				DMA2SAD_L = val;
				break;
			  case 0xCA:
				DMA2SAD_H = val & 0x0FFF;
				break;
			  case 0xCC:
				DMA2DAD_L = val;
				checkstart();
				break;
			  case 0xCE:
				DMA2DAD_H = val & 0x07FF;
				checkstart();
			  case 0xD0:
				//DMA2CNT_L = val & 0x3FFF;
				break;
			  case 0xD2:
				DMA2CNT_H = val;
				checkstart();
				break;





			case 0x100:
				TM0CNT_L = val;
				updatetakt();
				break;
			case 0x102:
				TM0CNT_H = val;
				updatetakt();
				checkstart();
				break;
			case 0x104:
				TM1CNT_L = val;
				updatetakt();
				break;
			case 0x106:
				TM1CNT_H = val;
				updatetakt();
				checkstart();
				break;
			case 0x80:
				SOUNDCNT_L = val;
				updatevol();
				break;
			case 0x82:
				SOUNDCNT_H = val; //Reset FIFO is not needed because we don't have direct streaming yet so don't need that
				updatevol();

				if(val & BIT(10))
				{
					tacktgeber_sound_FIFO_DMA_A = 1;
				}
				else
				{
					tacktgeber_sound_FIFO_DMA_A = 0;
				}
				if(val & BIT(14))
				{
					tacktgeber_sound_FIFO_DMA_B = 1;
				}
				else
				{
					tacktgeber_sound_FIFO_DMA_B = 0;
				}
				updatetakt();
				checkstart();
				break;
			case 0x84:
				if(val & 0x80)REG_SOUNDCNT |= 0x8000;
				else
				{
					REG_SOUNDCNT &= ~0x8000;
				}
				break;
			case 0x88:
					  //Amplitude Resolution/Sampling Cycle is not supported so only Bias
					  //it is better on the DS any way
				  REG_SOUNDBIAS = val;
				  break;



			  case 0x90:
			  case 0x92:
			  case 0x94:
			  case 0x96:
			  case 0x98:
			  case 0x9a:
			  case 0x9c:
			  case 0x9e:
				  {
					  u32 valtostr =  ((u8)((((val &0xF0) << 0) + ((val &0xF0) >> 4)) - 0x80) << 0);
					  valtostr |=  ((u8)((((val &0xF) << 0) + ((val &0xF) << 4)) - 0x80) << 8);
					  valtostr |=  ((u8)((((val &0xF000) >> 8) + ((val &0xF000) >> 0xC)) - 0x80) << 0x10);
					  valtostr |=  ((u8)((((val &0xF00) << 4) + ((val &0xF00) << 8)) - 0x80) << 0x18);
					  if(val&0x40)
					  {
							*(u32*)&WAVE_RAM[(addr - 0x90)*2] = valtostr;
							*(u32*)&WAVE_RAM[(addr - 0x70)*2] = valtostr;
					  }
					  else
					  {
							*(u32*)&WAVE_RAM[(addr - 0x80)*2] = valtostr;
					  }
				  }
				break;

			  case 0x1FFFFFF8: //wifi startup
				
				installWifiFIFO();
				REG_IPC_FIFO_TX = 0x100;
				if(!netinter->Wifi_InitDefault(true /*WFC_CONNECT*/))
				{
					REG_IPC_FIFO_TX = 0x201;
					break;
				}
				REG_IPC_FIFO_TX = 0x101;

//start socket



				struct sockaddr_in echoServAddr; /* Echo server address */
				unsigned short echoServPort;     /* Echo server port */
				char *servIP;                    /* Server IP address (dotted quad) */
				char *echoString;                /* String to send to echo server */
				unsigned int echoStringLen;      /* Length of string to echo */


    servIP = "192.168.168.35";             /* First arg: server IP address (dotted quad) */
    echoString = "handshake";         /* Second arg: string to echo */
	echoStringLen = 10;          /* 
								 Determine input length */

    echoServPort = 9876; /*port*/
    /* Create a reliable, stream socket using TCP */
    if ((sock = netinter->socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		REG_IPC_FIFO_TX = 0x202;
		break;
	}
    /* Construct the server address structure */
    //memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = netinter->inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = netinter->htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (netinter->connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
	{
		REG_IPC_FIFO_TX = 0x203;
		break;
	}

    /* Send the string to the server */
    if (netinter->send(sock, echoString, echoStringLen, 0) != echoStringLen)
	{
		REG_IPC_FIFO_TX = 0x204;
		break;
	}

	REG_IPC_FIFO_TX = 0x1;
	 break;

			  case 0x1FFFFFF9:
				writePowerManagement(0,(int)val);
				 break;
			  case 0x1FFFFFFA:
				//senddebug32(val);

				  dmabuffer = val;
#ifdef anyarmcom
				amr7sendcom =  (u32*)(*(u32*)(dmabuffer));
				amr7senddma1 = (u32*)(*(u32*)(dmabuffer + 4));
				amr7senddma2 = (u32*)(*(u32*)(dmabuffer + 8));
				amr7recmuell = (u32*)(*(u32*)(dmabuffer + 12));
				amr7directrec = (u32*)(*(u32*)(dmabuffer + 16));
				amr7indirectrec = (u32*)(*(u32*)(dmabuffer + 20));
				amr7fehlerfeld = (u32*)(*(u32*)(dmabuffer + 24));
#endif
				pointer_7Bitpoly = (u32)(*(u32*)(dmabuffer + 0x50));
				pointer_15Bitpoly = (u32)(*(u32*)(dmabuffer + 0x54));
				soundbuffA = (u8*)(dmabuffer);
				SCHANNEL_SOURCE(4) = (u32)soundbuffA;
				soundbuffB = (u8*)(dmabuffer + 0x50);
				SCHANNEL_SOURCE(5) = (u32)soundbuffB;
				WAVE_RAM = (u8*)(dmabuffer + 0x100);

			break;
			case WaitforVblancarmcmd: //wait
				if(autodetectdetect  && (REG_KEYXY & 0x1) /* && (REG_VCOUNT > 160 || REG_VCOUNT < callline)*/ )
				{
					REG_IPC_FIFO_TX = 0x4100BEEF; //send cmd 0x4100BEEF
#ifdef anyarmcom
	*amr7sendcom = *amr7sendcom + 1;
#endif
				}
				break;
			case enableWaitforVblancarmcmdirq: //setauto
				autodetectdetect = true;
				break;
			/*case getarm7keys: //getkeys
				{
					touchPosition tempPos = {0};
					u16 keys= REG_KEYXY;
					if(!touchPenDown()) {
						keys |= KEY_TOUCH;
  					} else {
						keys &= ~KEY_TOUCH;
					}
					touchReadXY(&tempPos);	
					REG_IPC_FIFO_TX = 1; //send cmd 1
					REG_IPC_FIFO_TX = keys;
					REG_IPC_FIFO_TX = tempPos.px;
					REG_IPC_FIFO_TX = tempPos.py;
				}
				break;*/
			case set_callline: //set callline
				irqSet(IRQ_VCOUNT, sendVcount);
				//callline = val;
				SetYtrigger(val);
				break;
			default:
#ifdef anyarmcom
	*amr7recmuell = *amr7recmuell + 1;
	amr7fehlerfeld[currfehler] = addr;
	amr7fehlerfeld[currfehler + 1] = val;
	currfehler+= 2;
	if(currfehler == maxfehler)currfehler = 0;
#endif
				break;
			}
}



void sendVcount()
{
	REG_IPC_FIFO_TX = 0x3F00BEEF; //send cmd 0x3F00BEEF
#ifdef anyarmcom
	*amr7sendcom = *amr7sendcom + 1;
#endif
}
void closelib()
{
	u32 ie_save = REG_IE;
	// Turn the speaker down.
	if (REG_POWERCNT & 1) swiChangeSoundBias(0,0x400);
	// Save current power state.
	u32 power = readPowerManagement(PM_CONTROL_REG);
	// Set sleep LED.
	writePowerManagement(PM_CONTROL_REG, (int)PM_LED_CONTROL(1));
	// Register for the lid interrupt.
	REG_IE = IRQ_LID;
	// Power down till we get our interrupt.
	swiSleep(); //waits for PM (lid open) interrupt
	//100ms
	swiDelay(838000);
	// Restore the interrupt state.
	REG_IE = ie_save;
	// Restore power state.
	writePowerManagement(PM_CONTROL_REG, (int)power);
	// Turn the speaker up.
	if (REG_POWERCNT & 1) swiChangeSoundBias(1,0x400);
	// update clock tracking
	resyncClock();
}
void FIFOhand()
{

		while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
		{
			u32 addr = (REG_IPC_FIFO_RX); //addr + flags //flags 2 most upperen Bits unknown = 0 u8 = 1 u16 = 2 u32 = 3
			//senddebug32(addr);
			if((addr& 0xC0000000) == 0x80000000)
			{
				newvalwrite16(addr & 0xFFF,(addr & ~0xF0000FFF) >> 12);
			}
			else if((addr& 0xC0000000) == 0x40000000)
			{
				newvalwrite8(addr & ~0xFFFFF000,(addr & ~0xF0000000) >> 12);
			}
			else if((addr& 0xC0000000) == 0xC0000000)
			{
				while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
				u32 val = REG_IPC_FIFO_RX; //Value skip add for speedup
				newvalwrite16(addr & ~0xC0000000,val);
			}
#ifdef anyarmcom
			*amr7directrec = *amr7directrec + 1;
			if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))*amr7indirectrec = *amr7indirectrec + 1;
#endif
		}

}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	readUserSettings();
	ledBlink(0);
	
	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();

	enableSound();

	irqSet(IRQ_FIFO_NOT_EMPTY, FIFOhand);
	irqSet(IRQ_VBLANK, VblankHandler);
	//irqSet(IRQ_LID, closelib);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_FIFO_NOT_EMPTY | IRQ_NETWORK);

	initimer();

	REG_IPC_SYNC = 0;
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE;
	REG_IPC_FIFO_CR = IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ | IPC_FIFO_ENABLE | IPC_FIFO_ERROR;



	SCHANNEL_REPEAT_POINT(4) = 0;
	SCHANNEL_LENGTH(4) = 8;

	SCHANNEL_REPEAT_POINT(5) = 0;
	SCHANNEL_LENGTH(5) = 8;

	while (true) {
		swiWaitForVBlank();
		//wifi debug
	    /* Receive*/



		if(sock != 0)
		{

			unsigned int echoStringLen = 4 + 4 + 4;      /* Length of cmd*/
			char echoBuffer[echoStringLen];     /* Buffer for echo string */
			int bytesRcvd, totalBytesRcvd = 0;   /* Bytes read in single recv() 
												and total bytes read */
			while (totalBytesRcvd < echoStringLen)
			{
				if ((bytesRcvd = netinter->recv(sock, &echoBuffer[totalBytesRcvd], echoStringLen, 0)) <= 0)
				{
					sendwififail32(0x3);
				}
				totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
			}
			//cmd 0 = keepalive
			if(echoBuffer[0] == 1) //cmd read
			{
				u32 buff1 = *(u32*)(&echoBuffer[4]);
				u32 buff2 = *(u32*)(&echoBuffer[8]);
				if (netinter->send(sock, (void*)(buff1),buff2 , 0) != buff2)
				{
					sendwififail32(0x2);
				}
			}
			else if(echoBuffer[0] == 2) //cmd write
			{
				u32 buff1 = *(u32*)(&echoBuffer[4]);
				u32 buff2 = *(u32*)(&echoBuffer[8]);

				bytesRcvd, totalBytesRcvd = 0;
				unsigned int echoStringLen = buff2;
				while (totalBytesRcvd < echoStringLen)
				{
					if ((bytesRcvd = netinter->recv(sock, (void*)(buff1 + totalBytesRcvd), 0x100, 0)) <= 0)
					{
						sendwififail32(0x1);
					}
					totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
				}	
			}
		}

	}
	return 0;
}
//global update stuff

void updatevol()
{

	/*****************************************************/
	/*                  Update Voll                      */
	/*****************************************************/


	//Sound_chan = (Volume_Right * enabeled + Volume_Left * enabeled) * (Soundcnt(1,2,4))*static_for_max
	//DMA = (Soundcnt(1,2) * enabeled + Soundcnt(1,2) * enabeled) * sataic_for_max
	SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF) | ((( 1 + ((SOUNDCNT_H & 0x4) >> 2))*((SOUNDCNT_H & BIT(8)) >> 8) + ( 1 + ((SOUNDCNT_H & 0x4) >> 2))*((SOUNDCNT_H & BIT(9)) >> 9))*127/4);     
	SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF) | ((( 1 + ((SOUNDCNT_H & 0x8) >> 3))*((SOUNDCNT_H & BIT(12)) >> 12) + ( 1 + ((SOUNDCNT_H & 0x8) >> 3))*((SOUNDCNT_H & BIT(13)) >> 13))*127/4);
	u32 Vol = SOUNDCNT_H & 0x3;
	switch(Vol)
	{
	case 3:
	case 0:
		Vol = 1;
		break;
	case 1:
		Vol = 2;
		break;
	case 2:
		Vol = 4;
		break;
	}
	u32 Masterright = SOUNDCNT_L & 0x7;
	u32 Masterleft =  (SOUNDCNT_L >> 4) & 0x7;
	SCHANNEL_CR(8) = ((SCHANNEL_CR(8) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(8)) >> 8) + Masterleft * ((SOUNDCNT_L & BIT(12)) >> 12) ) * Vol) * ((SOUND1CNT_X & 0xF000) >> 12)*127/840);
	SCHANNEL_CR(9) = ((SCHANNEL_CR(9) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(9)) >> 9) + Masterleft * ((SOUNDCNT_L & BIT(13)) >> 13) ) * Vol) * ((SOUND2CNT_H & 0xF000) >> 12)*127/840);
	SCHANNEL_CR(3) = ((SCHANNEL_CR(3) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(11)) >> 11) + Masterleft * ((SOUNDCNT_L & BIT(15)) >> 15) )* Vol) * ((SOUND4CNT_L & 0xF000) >> 12)*127/840);


	u16 sound3dif = (SOUND3CNT_H & 0x6000) >> 14;
	u16 sound3mul = 1;
	if (sound3dif = 0)
	{
		sound3dif = 1;
		sound3mul = 0;
	}
	if(SOUND3CNT_H & 0x8000)
	{
		sound3dif = 4;
		sound3mul = 3;
	}
	SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(10)) >> 10) + Masterleft * ((SOUNDCNT_L & BIT(14)) >> 14) ) * Vol * 2 * sound3mul / sound3dif*127/112 );



    /*****************************************************/
	/*                  Update Panning                   */
	/*****************************************************/
	

	if(SOUNDCNT_H & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000);
	if(SOUNDCNT_H & BIT(8)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_H & BIT(8) && SOUNDCNT_H & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x400000; //same on both

	if(SOUNDCNT_H & BIT(13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000);
	if(SOUNDCNT_H & BIT(12)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_H & BIT(12) && SOUNDCNT_H & BIT(13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x400000; //same on both

	u32 right = SOUNDCNT_L & 7;
	u32 left = (SOUNDCNT_L << 4) & 7;
	u32 tempmixedvol1_4 = 0;
	if((left + right) != 0) //don't work
	{
		tempmixedvol1_4 = ((right*0x7F0000)/(right + left) & 0x7F0000);
	}
	
	if(SOUNDCNT_L & BIT(12)) SCHANNEL_CR(8) = (SCHANNEL_CR(8) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(8)) SCHANNEL_CR(8) = (SCHANNEL_CR(8) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(8) && SOUNDCNT_L & BIT(12)) SCHANNEL_CR(8) = (SCHANNEL_CR(8) & ~0xFF0000) | tempmixedvol1_4;

	if(SOUNDCNT_L & BIT(13)) SCHANNEL_CR(9) = (SCHANNEL_CR(9) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(9)) SCHANNEL_CR(9) = (SCHANNEL_CR(9) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(9) && SOUNDCNT_L & BIT(13)) SCHANNEL_CR(9) = (SCHANNEL_CR(9) & ~0xFF0000) | tempmixedvol1_4; 

	if(SOUNDCNT_L & BIT(14)) SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(10)) SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(10) && SOUNDCNT_L & BIT(14)) SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF0000) | tempmixedvol1_4;

	if(SOUNDCNT_L & BIT(15)) SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(11)) SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(11) && SOUNDCNT_L & BIT(15)) SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF0000) | tempmixedvol1_4;

}


void updatetakt()
{
	//FIFO A
	if(tacktgeber_sound_FIFO_DMA_A == 0)
	{
		u8 seek;
		switch(TM0CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(4) = debugfr1 = TM0CNT_L << seek;
	}
	else
	{
		u8 seek;
		switch(TM1CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(4) = debugfr1 = TM1CNT_L  << seek;
	}
	//FIFO B
	if(tacktgeber_sound_FIFO_DMA_B == 0)
	{
		u8 seek;
		switch(TM0CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(5) = debugfr2 = TM0CNT_L  << seek;
	}
	else
	{
		u8 seek;
		switch(TM1CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(5) = debugfr2 = TM1CNT_L  << seek;
	}
}

void checkstart()
{
	//DMA 1
	if(DMA1DAD_L == 0x00A0 && DMA1DAD_H == 0x0400 && (DMA1CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA1SAD_L | (DMA1SAD_H << 16);
				//senddebug32(debugsrc1);
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now if not already started
				//senddebug32(SCHANNEL_CR(4));
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				////senddebug32(debugfr1 << 5);
			}
		}
		else
		{
			if(TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA1SAD_L | (DMA1SAD_H << 16);
				//senddebug32(debugsrc1);
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				//senddebug32(SCHANNEL_CR(4));
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
		}
	}
	else
	{
		if(DMA1DAD_L == 0x00A4 && DMA1DAD_H == 0x0400 && (DMA1CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA1SAD_L | (DMA1SAD_H << 16);
					//senddebug32(debugsrc2);
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
			}
			else
			{
				if(TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA1SAD_L | (DMA1SAD_H << 16);
					//senddebug32(debugsrc2);
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
			}
		}

	}


	//DMA 2
	if(DMA2DAD_L == 0x00A0 && DMA2DAD_H == 0x0400 && (DMA2CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA2SAD_L | (DMA2SAD_H << 16);
				//senddebug32(debugsrc1);
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				//senddebug32(SCHANNEL_CR(4));
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
		}
		else
		{
			if(TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA2SAD_L | (DMA2SAD_H << 16);
				//senddebug32(debugsrc1);
				if(!(SCHANNEL_CR(4) & 0x80000000))SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				//senddebug32(SCHANNEL_CR(4));
				if(!(TIMER0_CR & TIMER_ENABLE))timerStart(0, ClockDivider_1,debugfr1 << 5, dmaAtimerinter);
				//senddebug32(debugfr1 << 5);
			}
		}
	}
	else
	{
		if(DMA2DAD_L == 0x00A4 && DMA2DAD_H == 0x0400 && (DMA2CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA2SAD_L | (DMA2SAD_H << 16);
					//senddebug32(debugsrc2);
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				//senddebug32(debugfr2 << 5);
				}
			}
			else
			{
				if(TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA2SAD_L | (DMA2SAD_H << 16);
					//senddebug32(debugsrc2);
					if(!(SCHANNEL_CR(5) & 0x80000000))SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					if(!(TIMER1_CR & TIMER_ENABLE))timerStart(1, ClockDivider_1,debugfr2 << 5, dmaBtimerinter);
				}
			}
		}

	}



}

//---------------------------------------------------------------------------------
void enableSound() {
//---------------------------------------------------------------------------------
	powerOn(POWER_SOUND);
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	REG_SOUNDCNT = SOUND_ENABLE;
	REG_MASTER_VOLUME = 127;
}
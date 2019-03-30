/**
 * Gameshark code types: (based on AR v1.0)
 *
 * NNNNNNNN 001DC0DE - ID code for the game (game 4 character name) from ROM
 * DEADFACE XXXXXXXX - changes decryption seeds // Not supported by VBA.
 * 0AAAAAAA 000000YY - 8-bit constant write
 * 1AAAAAAA 0000YYYY - 16-bit constant write
 * 2AAAAAAA YYYYYYYY - 32-bit constant write
 * 30XXAAAA YYYYYYYY - 32bit Group Write, 8/16/32bit Sub/Add (depending on the XX value).
 * 6AAAAAAA Z000YYYY - 16-bit ROM Patch (address >> 1). Z selects the Rom Patching register.
 *                   - AR v1/2 hardware only supports Z=0.
 *                   - AR v3 hardware should support Z=0,1,2 or 3.
 * 8A1AAAAA 000000YY - 8-bit button write
 * 8A2AAAAA 0000YYYY - 16-bit button write
 * 8A4AAAAA YYYYYYYY - 32-bit button write // BUGGY ! Only writes 00000000 on the AR v1.0.
 * 80F00000 0000YYYY - button slow motion
 * DAAAAAAA 00Z0YYYY - Z = 0 : if 16-bit value at address != YYYY skip next line
 *                   - Z = 1 : if 16-bit value at address == YYYY skip next line
 *                   - Z = 2 : if 16-bit value at address > YYYY (Unsigned) skip next line
 *                   - Z = 3 : if 16-bit value at address < YYYY (Unsigned) skip next line
 * E0CCYYYY ZAAAAAAA - Z = 0 : if 16-bit value at address != YYYY skip CC lines
 *                   - Z = 1 : if 16-bit value at address == YYYY skip CC lines
 *                   - Z = 2 : if 16-bit value at address > YYYY (Unsigned) skip CC lines
 *                   - Z = 3 : if 16-bit value at address < YYYY (Unsigned) skip CC lines
 * FAAAAAAA 0000YYYY - Master code function
 *
 *
 *
 * CodeBreaker codes types: (based on the CBA clone "Cheatcode S" v1.1)
 *
 * 0000AAAA 000Y - Game CRC (Y are flags: 8 - CRC, 2 - DI)
 * 1AAAAAAA YYYY - Master Code function (store address at ((YYYY << 0x16)
 *                 + 0x08000100))
 * 2AAAAAAA YYYY - 16-bit or
 * 3AAAAAAA YYYY - 8-bit constant write
 * 4AAAAAAA YYYY - Slide code
 * XXXXCCCC IIII   (C is count and I is address increment, X is value incr.)
 * 5AAAAAAA CCCC - Super code (Write bytes to address, 2*CCCC is count)
 * BBBBBBBB BBBB 
 * 6AAAAAAA YYYY - 16-bit and
 * 7AAAAAAA YYYY - if address contains 16-bit value enable next code
 * 8AAAAAAA YYYY - 16-bit constant write
 * 9AAAAAAA YYYY - change decryption (when first code only?)
 * AAAAAAAA YYYY - if address does not contain 16-bit value enable next code
 * BAAAAAAA YYYY - if 16-bit value at address  <= YYYY skip next code
 * CAAAAAAA YYYY - if 16-bit value at address  >= YYYY skip next code
 * D00000X0 YYYY - if button keys ... enable next code (else skip next code)
 * EAAAAAAA YYYY - increase 16/32bit value stored in address
 * FAAAAAAA YYYY - if 16-bit value at address AND YYYY = 0 then skip next code
 **/

#define u32 unsigned int
#define u16 unsigned short
#define u8  unsigned char
#define s32 signed   int
#define s16 signed   short
#define s8  signed   char

#include "Cheats.h"
#include "NLS.h"

u32 mastercode = 0;


#define UNKNOWN_CODE                  -1
#define INT_8_BIT_WRITE               0
#define INT_16_BIT_WRITE              1
#define INT_32_BIT_WRITE              2
#define GSA_16_BIT_ROM_PATCH          3
#define GSA_8_BIT_GS_WRITE            4
#define GSA_16_BIT_GS_WRITE           5
#define GSA_32_BIT_GS_WRITE           6
#define CBA_IF_KEYS_PRESSED           7
#define CBA_IF_TRUE                   8
#define CBA_SLIDE_CODE                9
#define CBA_IF_FALSE                  10
#define CBA_AND                       11
#define GSA_8_BIT_GS_WRITE2           12
#define GSA_16_BIT_GS_WRITE2          13
#define GSA_32_BIT_GS_WRITE2          14
#define GSA_16_BIT_ROM_PATCH2C        15
#define GSA_8_BIT_SLIDE               16
#define GSA_16_BIT_SLIDE              17
#define GSA_32_BIT_SLIDE              18
#define GSA_8_BIT_IF_TRUE             19
#define GSA_32_BIT_IF_TRUE            20
#define GSA_8_BIT_IF_FALSE            21
#define GSA_32_BIT_IF_FALSE           22
#define GSA_8_BIT_FILL                23
#define GSA_16_BIT_FILL               24
#define GSA_8_BIT_IF_TRUE2            25
#define GSA_16_BIT_IF_TRUE2           26
#define GSA_32_BIT_IF_TRUE2           27
#define GSA_8_BIT_IF_FALSE2           28
#define GSA_16_BIT_IF_FALSE2          29
#define GSA_32_BIT_IF_FALSE2          30
#define GSA_SLOWDOWN                  31
#define CBA_ADD                       32
#define CBA_OR                        33
#define CBA_LT                        34
#define CBA_GT                        35 
#define CBA_SUPER                     36
#define GSA_8_BIT_POINTER             37
#define GSA_16_BIT_POINTER            38
#define GSA_32_BIT_POINTER            39
#define GSA_8_BIT_ADD                 40
#define GSA_16_BIT_ADD                41
#define GSA_32_BIT_ADD                42
#define GSA_8_BIT_IF_LOWER_U          43
#define GSA_16_BIT_IF_LOWER_U         44
#define GSA_32_BIT_IF_LOWER_U         45
#define GSA_8_BIT_IF_HIGHER_U         46
#define GSA_16_BIT_IF_HIGHER_U        47
#define GSA_32_BIT_IF_HIGHER_U        48
#define GSA_8_BIT_IF_AND              49
#define GSA_16_BIT_IF_AND             50
#define GSA_32_BIT_IF_AND             51
#define GSA_8_BIT_IF_LOWER_U2         52
#define GSA_16_BIT_IF_LOWER_U2        53
#define GSA_32_BIT_IF_LOWER_U2        54
#define GSA_8_BIT_IF_HIGHER_U2        55
#define GSA_16_BIT_IF_HIGHER_U2       56
#define GSA_32_BIT_IF_HIGHER_U2       57
#define GSA_8_BIT_IF_AND2             58
#define GSA_16_BIT_IF_AND2            59
#define GSA_32_BIT_IF_AND2            60
#define GSA_ALWAYS                    61
#define GSA_ALWAYS2                   62
#define GSA_8_BIT_IF_LOWER_S          63
#define GSA_16_BIT_IF_LOWER_S         64
#define GSA_32_BIT_IF_LOWER_S         65
#define GSA_8_BIT_IF_HIGHER_S         66
#define GSA_16_BIT_IF_HIGHER_S        67
#define GSA_32_BIT_IF_HIGHER_S        68
#define GSA_8_BIT_IF_LOWER_S2         69
#define GSA_16_BIT_IF_LOWER_S2        70
#define GSA_32_BIT_IF_LOWER_S2        71
#define GSA_8_BIT_IF_HIGHER_S2        72
#define GSA_16_BIT_IF_HIGHER_S2       73
#define GSA_32_BIT_IF_HIGHER_S2       74
#define GSA_16_BIT_WRITE_IOREGS       75
#define GSA_32_BIT_WRITE_IOREGS       76
#define GSA_CODES_ON                  77
#define GSA_8_BIT_IF_TRUE3            78
#define GSA_16_BIT_IF_TRUE3           79
#define GSA_32_BIT_IF_TRUE3           80
#define GSA_8_BIT_IF_FALSE3           81
#define GSA_16_BIT_IF_FALSE3          82
#define GSA_32_BIT_IF_FALSE3          83
#define GSA_8_BIT_IF_LOWER_S3         84
#define GSA_16_BIT_IF_LOWER_S3        85
#define GSA_32_BIT_IF_LOWER_S3        86
#define GSA_8_BIT_IF_HIGHER_S3        87
#define GSA_16_BIT_IF_HIGHER_S3       88
#define GSA_32_BIT_IF_HIGHER_S3       89
#define GSA_8_BIT_IF_LOWER_U3         90
#define GSA_16_BIT_IF_LOWER_U3        91
#define GSA_32_BIT_IF_LOWER_U3        92
#define GSA_8_BIT_IF_HIGHER_U3        93
#define GSA_16_BIT_IF_HIGHER_U3       94
#define GSA_32_BIT_IF_HIGHER_U3       95
#define GSA_8_BIT_IF_AND3             96
#define GSA_16_BIT_IF_AND3            97
#define GSA_32_BIT_IF_AND3            98
#define GSA_ALWAYS3                   99
#define GSA_16_BIT_ROM_PATCH2D        100
#define GSA_16_BIT_ROM_PATCH2E        101
#define GSA_16_BIT_ROM_PATCH2F        102
#define GSA_GROUP_WRITE               103
#define GSA_32_BIT_ADD2               104
#define GSA_32_BIT_SUB2               105
#define GSA_16_BIT_IF_LOWER_OR_EQ_U   106
#define GSA_16_BIT_IF_HIGHER_OR_EQ_U  107
#define GSA_16_BIT_MIF_TRUE           108
#define GSA_16_BIT_MIF_FALSE          109
#define GSA_16_BIT_MIF_LOWER_OR_EQ_U  110
#define GSA_16_BIT_MIF_HIGHER_OR_EQ_U 111
#define MASTER_CODE                   112
#define CHEATS_16_BIT_WRITE           114
#define CHEATS_32_BIT_WRITE           115

struct CheatsData cheatsList[100];
int cheatsNumber = 0;
u32 rompatch2addr [4];
u16 rompatch2val [4];
u16 rompatch2oldval [4];

#define debuggerReadMemory(addr) \
  READ32LE(((uint32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define debuggerReadHalfWord(addr) \
  READ16LE(((uint16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define debuggerReadByte(addr) \
  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]

#define debuggerWriteMemory(addr, value) \
  WRITE32LE((uint32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask], value)

#define debuggerWriteHalfWord(addr, value) \
  WRITE16LE((uint16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask], value)

#define debuggerWriteByte(addr, value) \
  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask] = (value)

#define ioMem ((u16*)0x4000000)

#define CHEAT_IS_HEX(a) ( ((a)>='A' && (a) <='F') || ((a) >='0' && (a) <= '9'))

#define CHEAT_PATCH_ROM_16BIT(a,v) \
//  WRITE16LE(((u16 *)&rom[(a) & 0x1ffffff]), v);

#define CHEAT_PATCH_ROM_32BIT(a,v) \
//  WRITE32LE(((u32 *)&rom[(a) & 0x1ffffff]), v);  


inline void CPUWriteMemory(u32 address, u32 value)
{
	*(u32*)address = value;
}
inline u32 CPUReadMemory(u32 address)
{
	return *(u32*)address;
}
inline u32 CPUReadHalfWord(u32 address)
{
	return (u32)(*(u16*)address);
}
inline void CPUWriteHalfWord(u32 address, u16 value)
{
	*(u16*)address = value;
}
inline void CPUWriteByte(u32 address, u8 b)
{
	*(u8*)address = b;
}
inline u8 CPUReadByte(u32 address)
{
	return *(u8*)address;
}

static int getCodeLength(int num)
{
  if(num >= cheatsNumber || num < 0)
    return 1;

  // this is for all the codes that are true multiline
  switch(cheatsList[num].size) {
  case INT_8_BIT_WRITE:
  case INT_16_BIT_WRITE:
  case INT_32_BIT_WRITE:
  case GSA_16_BIT_ROM_PATCH:
  case GSA_8_BIT_GS_WRITE:
  case GSA_16_BIT_GS_WRITE:
  case GSA_32_BIT_GS_WRITE:
  case CBA_AND:
  case GSA_8_BIT_FILL:
  case GSA_16_BIT_FILL:
  case GSA_SLOWDOWN:
  case CBA_ADD:
  case CBA_OR:
  case GSA_8_BIT_POINTER:
  case GSA_16_BIT_POINTER:
  case GSA_32_BIT_POINTER:
  case GSA_8_BIT_ADD:
  case GSA_16_BIT_ADD:
  case GSA_32_BIT_ADD:
  case GSA_CODES_ON:
  case GSA_8_BIT_IF_TRUE3:
  case GSA_16_BIT_IF_TRUE3:
  case GSA_32_BIT_IF_TRUE3:
  case GSA_8_BIT_IF_FALSE3:
  case GSA_16_BIT_IF_FALSE3:
  case GSA_32_BIT_IF_FALSE3:
  case GSA_8_BIT_IF_LOWER_S3:
  case GSA_16_BIT_IF_LOWER_S3:
  case GSA_32_BIT_IF_LOWER_S3:
  case GSA_8_BIT_IF_HIGHER_S3:
  case GSA_16_BIT_IF_HIGHER_S3:
  case GSA_32_BIT_IF_HIGHER_S3:
  case GSA_8_BIT_IF_LOWER_U3:
  case GSA_16_BIT_IF_LOWER_U3:
  case GSA_32_BIT_IF_LOWER_U3:
  case GSA_8_BIT_IF_HIGHER_U3:
  case GSA_16_BIT_IF_HIGHER_U3:
  case GSA_32_BIT_IF_HIGHER_U3:
  case GSA_8_BIT_IF_AND3:
  case GSA_16_BIT_IF_AND3:
  case GSA_32_BIT_IF_AND3:
  case GSA_8_BIT_IF_LOWER_U:
  case GSA_16_BIT_IF_LOWER_U:
  case GSA_32_BIT_IF_LOWER_U:
  case GSA_8_BIT_IF_HIGHER_U:
  case GSA_16_BIT_IF_HIGHER_U:
  case GSA_32_BIT_IF_HIGHER_U:
  case GSA_8_BIT_IF_AND:
  case GSA_16_BIT_IF_AND:
  case GSA_32_BIT_IF_AND:
  case GSA_ALWAYS:
  case GSA_8_BIT_IF_LOWER_S:
  case GSA_16_BIT_IF_LOWER_S:
  case GSA_32_BIT_IF_LOWER_S:
  case GSA_8_BIT_IF_HIGHER_S:
  case GSA_16_BIT_IF_HIGHER_S:
  case GSA_32_BIT_IF_HIGHER_S:
  case GSA_16_BIT_WRITE_IOREGS:
  case GSA_32_BIT_WRITE_IOREGS:
  case GSA_8_BIT_GS_WRITE2:
  case GSA_16_BIT_GS_WRITE2:
  case GSA_32_BIT_GS_WRITE2:
  case GSA_16_BIT_ROM_PATCH2C:
  case GSA_16_BIT_ROM_PATCH2D:
  case GSA_16_BIT_ROM_PATCH2E:
  case GSA_16_BIT_ROM_PATCH2F:
  case GSA_8_BIT_SLIDE:
  case GSA_16_BIT_SLIDE:
  case GSA_32_BIT_SLIDE:
  case GSA_8_BIT_IF_TRUE:
  case GSA_32_BIT_IF_TRUE:
  case GSA_8_BIT_IF_FALSE:
  case GSA_32_BIT_IF_FALSE:
  case CBA_LT:
  case CBA_GT:
  case CBA_IF_TRUE:
  case CBA_IF_FALSE:
  case GSA_8_BIT_IF_TRUE2:
  case GSA_16_BIT_IF_TRUE2:
  case GSA_32_BIT_IF_TRUE2:
  case GSA_8_BIT_IF_FALSE2:
  case GSA_16_BIT_IF_FALSE2:
  case GSA_32_BIT_IF_FALSE2:
  case GSA_8_BIT_IF_LOWER_U2:
  case GSA_16_BIT_IF_LOWER_U2:
  case GSA_32_BIT_IF_LOWER_U2:
  case GSA_8_BIT_IF_HIGHER_U2:
  case GSA_16_BIT_IF_HIGHER_U2:
  case GSA_32_BIT_IF_HIGHER_U2:
  case GSA_8_BIT_IF_AND2:
  case GSA_16_BIT_IF_AND2:
  case GSA_32_BIT_IF_AND2:
  case GSA_ALWAYS2:
  case GSA_8_BIT_IF_LOWER_S2:
  case GSA_16_BIT_IF_LOWER_S2:
  case GSA_32_BIT_IF_LOWER_S2:
  case GSA_8_BIT_IF_HIGHER_S2:
  case GSA_16_BIT_IF_HIGHER_S2:
  case GSA_32_BIT_IF_HIGHER_S2:
  case GSA_GROUP_WRITE:
  case GSA_32_BIT_ADD2:
  case GSA_32_BIT_SUB2:
  case GSA_16_BIT_IF_LOWER_OR_EQ_U:
  case GSA_16_BIT_IF_HIGHER_OR_EQ_U:
  case GSA_16_BIT_MIF_TRUE:
  case GSA_16_BIT_MIF_FALSE:
  case GSA_16_BIT_MIF_LOWER_OR_EQ_U:
  case GSA_16_BIT_MIF_HIGHER_OR_EQ_U:
  case MASTER_CODE:
  case CHEATS_16_BIT_WRITE:
  case CHEATS_32_BIT_WRITE:
  case UNKNOWN_CODE:
    return 1;
  case CBA_IF_KEYS_PRESSED:
  case CBA_SLIDE_CODE:
    return 2;
  case CBA_SUPER:
    return ((((cheatsList[num].value-1) & 0xFFFF)/3) + 1);
  }
  return 1;
}



int cheatsCheckKeys()
{
  u32 extended = 0; //0 bis ich es verstehe
  bool onoff = true;
  int ticks = 0;
  int i;
  mastercode = 0;

  for (i = 0; i<4; i++)
    if (rompatch2addr [i] != 0) {
      CHEAT_PATCH_ROM_16BIT(rompatch2addr [i],rompatch2oldval [i]);
      rompatch2addr [i] = 0;
    }

  for (i = 0; i < cheatsNumber; i++) {
    if(!cheatsList[i].enabled) {
      // make sure we skip other lines in this code
      i += getCodeLength(i)-1;
      continue;
    }
    switch(cheatsList[i].size) {
    case GSA_CODES_ON:
      onoff = true;
      break;
    case GSA_SLOWDOWN:
      // check if button was pressed and released, if so toggle our state
      if((cheatsList[i].status & 4) && !(extended & 4))
        cheatsList[i].status ^= 1;
      if(extended & 4)
        cheatsList[i].status |= 4;
      else
        cheatsList[i].status &= ~4;
      
      if(cheatsList[i].status & 1)
        ticks += ((cheatsList[i].value  & 0xFFFF) * 7);
      break;
    case GSA_8_BIT_SLIDE:
      i++;
      if(i < cheatsNumber) {
        u32 addr = cheatsList[i-1].value;
        u8 value = cheatsList[i].rawaddress;
        int vinc = (cheatsList[i].value >> 24) & 255;
        int count = (cheatsList[i].value >> 16) & 255;
        int ainc = (cheatsList[i].value & 0xffff);
        while(count > 0) {
          CPUWriteByte(addr, value);
          value += vinc;
          addr += ainc;
          count--;
        }
      }
      break;
    case GSA_16_BIT_SLIDE:
      i++;
      if(i < cheatsNumber) {
        u32 addr = cheatsList[i-1].value;
        u16 value = cheatsList[i].rawaddress;
        int vinc = (cheatsList[i].value >> 24) & 255;
        int count = (cheatsList[i].value >> 16) & 255;
        int ainc = (cheatsList[i].value & 0xffff)*2;
        while(count > 0) {
          CPUWriteHalfWord(addr, value);
          value += vinc;
          addr += ainc;
          count--;
        }
      }
      break;
    case GSA_32_BIT_SLIDE:
      i++;
      if(i < cheatsNumber) {
        u32 addr = cheatsList[i-1].value;
        u32 value = cheatsList[i].rawaddress;
        int vinc = (cheatsList[i].value >> 24) & 255;
        int count = (cheatsList[i].value >> 16) & 255;
        int ainc = (cheatsList[i].value & 0xffff)*4;
        while(count > 0) {
          CPUWriteMemory(addr, value);
          value += vinc;
          addr += ainc;
          count--;
        }
      }
      break;
    case GSA_8_BIT_GS_WRITE2:
      i++;
      if(i < cheatsNumber) {
        if(extended & 4) {
          CPUWriteByte(cheatsList[i-1].value, cheatsList[i].address);
        }
      }
      break;
    case GSA_16_BIT_GS_WRITE2:
      i++;
      if(i < cheatsNumber) {
        if(extended & 4) {
          CPUWriteHalfWord(cheatsList[i-1].value, cheatsList[i].address);
        }
      }
      break;
    case GSA_32_BIT_GS_WRITE2:
      i++;
      if(i < cheatsNumber) {
        if(extended & 4) {
          CPUWriteMemory(cheatsList[i-1].value, cheatsList[i].address);
        }
      }
      break;
      case GSA_16_BIT_ROM_PATCH:
        if((cheatsList[i].status & 1) == 0) {
          if(CPUReadHalfWord(cheatsList[i].address) != cheatsList[i].value) {
            cheatsList[i].oldValue = CPUReadHalfWord(cheatsList[i].address);
            cheatsList[i].status |= 1;
            CHEAT_PATCH_ROM_16BIT(cheatsList[i].address, cheatsList[i].value);
          }
        }
        break;
    case GSA_16_BIT_ROM_PATCH2C:
      i++;
      if(i < cheatsNumber) {
		  rompatch2addr [0] = ((cheatsList[i-1].value & 0x00FFFFFF) << 1) + 0x8000000;
		  rompatch2oldval [0] = CPUReadHalfWord(rompatch2addr [0]);
		  rompatch2val [0] = cheatsList[i].rawaddress & 0xFFFF;
      }
      break;
    case GSA_16_BIT_ROM_PATCH2D:
      i++;
      if(i < cheatsNumber) {
		  rompatch2addr [1] = ((cheatsList[i-1].value & 0x00FFFFFF) << 1) + 0x8000000;
		  rompatch2oldval [1] = CPUReadHalfWord(rompatch2addr [1]);
		  rompatch2val [1] = cheatsList[i].rawaddress & 0xFFFF;
      }
      break;
    case GSA_16_BIT_ROM_PATCH2E:
      i++;
      if(i < cheatsNumber) {
		  rompatch2addr [2] = ((cheatsList[i-1].value & 0x00FFFFFF) << 1) + 0x8000000;
		  rompatch2oldval [2] = CPUReadHalfWord(rompatch2addr [2]);
		  rompatch2val [2] = cheatsList[i].rawaddress & 0xFFFF;
      }
      break;
    case GSA_16_BIT_ROM_PATCH2F:
      i++;
      if(i < cheatsNumber) {
		  rompatch2addr [3] = ((cheatsList[i-1].value & 0x00FFFFFF) << 1) + 0x8000000;
		  rompatch2oldval [3] = CPUReadHalfWord(rompatch2addr [3]);
		  rompatch2val [3] = cheatsList[i].rawaddress & 0xFFFF;
      }
      break;
    case MASTER_CODE:
        mastercode = cheatsList[i].address;
      break;
    }
    if (onoff) {
      switch(cheatsList[i].size) {
      case INT_8_BIT_WRITE:
        CPUWriteByte(cheatsList[i].address, cheatsList[i].value);
        break;
      case INT_16_BIT_WRITE:
        CPUWriteHalfWord(cheatsList[i].address, cheatsList[i].value);
        break;
      case INT_32_BIT_WRITE:
        CPUWriteMemory(cheatsList[i].address, cheatsList[i].value);
        break;
      case GSA_8_BIT_GS_WRITE:
        if(extended & 4) {
          CPUWriteByte(cheatsList[i].address, cheatsList[i].value);
        }
        break;
      case GSA_16_BIT_GS_WRITE:
        if(extended & 4) {
          CPUWriteHalfWord(cheatsList[i].address, cheatsList[i].value);   
        }
        break;
      case GSA_32_BIT_GS_WRITE:
        if(extended & 4) {
          CPUWriteMemory(cheatsList[i].address, cheatsList[i].value);     
        }
        break;
      case CBA_IF_KEYS_PRESSED:
        {
		  u32 keys = *(u16*)0x04000130;
          u16 value = cheatsList[i].value;
          u32 addr = cheatsList[i].address;
          if((addr & 0xF0) == 0x20) {
            if((keys & value) == 0) {
              i++;
			}
		  } else if((addr & 0xF0) == 0x10) {
            if((keys & value) == value) {
              i++;
			}
		  } else if((addr & 0xF0) == 0x00) {
            if(((~keys) & 0x3FF) == value) {
              i++;
			}
		  }
		}
        break;
      case CBA_IF_TRUE:
        if(CPUReadHalfWord(cheatsList[i].address) != cheatsList[i].value) {
          i++;
        }
        break;
      case CBA_SLIDE_CODE:
		{
          u32 address = cheatsList[i].address;
          u16 value = cheatsList[i].value;
          i++;
          if(i < cheatsNumber) {
            int count = ((cheatsList[i].address - 1) & 0xFFFF);
            u16 vinc = (cheatsList[i].address >> 16) & 0xFFFF;
            int inc = cheatsList[i].value;
            for(int x = 0; x <= count ; x++) {
              CPUWriteHalfWord(address, value);
              address += inc;
              value += vinc;
			}
		  }
		}
        break;
      case CBA_IF_FALSE:
        if(CPUReadHalfWord(cheatsList[i].address) == cheatsList[i].value){
          i++;
        }
      break;
      case CBA_AND:
        CPUWriteHalfWord(cheatsList[i].address,
                         CPUReadHalfWord(cheatsList[i].address) &
                         cheatsList[i].value);
        break;
      case GSA_8_BIT_IF_TRUE:
        if(CPUReadByte(cheatsList[i].address) != cheatsList[i].value) {
          i++;
        }
        break;
      case GSA_32_BIT_IF_TRUE:
        if(CPUReadMemory(cheatsList[i].address) != cheatsList[i].value) {
          i++;
        }
        break;
      case GSA_8_BIT_IF_FALSE:
        if(CPUReadByte(cheatsList[i].address) == cheatsList[i].value) {
          i++;
        }
        break;
      case GSA_32_BIT_IF_FALSE:
        if(CPUReadMemory(cheatsList[i].address) == cheatsList[i].value) {
          i++;
        }
        break;
      case GSA_8_BIT_FILL:
		{
          u32 addr = cheatsList[i].address;
          u8 v = cheatsList[i].value & 0xff;
          u32 end = addr + (cheatsList[i].value >> 8);
          do {
            CPUWriteByte(addr, v);
            addr++;
		  } while (addr <= end);
		}
        break;
      case GSA_16_BIT_FILL:
		{
          u32 addr = cheatsList[i].address;
          u16 v = cheatsList[i].value & 0xffff;
          u32 end = addr + ((cheatsList[i].value >> 16) << 1);
          do {
            CPUWriteHalfWord(addr, v);
            addr+=2;
		  } while (addr <= end);
		}
        break;
      case GSA_8_BIT_IF_TRUE2:
        if(CPUReadByte(cheatsList[i].address) != cheatsList[i].value) {
          i+=2;
        }
        break;
      case GSA_16_BIT_IF_TRUE2:
        if(CPUReadHalfWord(cheatsList[i].address) != cheatsList[i].value) {
          i+=2;
        }
        break;
      case GSA_32_BIT_IF_TRUE2:
        if(CPUReadMemory(cheatsList[i].address) != cheatsList[i].value) {
          i+=2;
        }
        break;
      case GSA_8_BIT_IF_FALSE2:
        if(CPUReadByte(cheatsList[i].address) == cheatsList[i].value) {
          i+=2;
        }
        break;
      case GSA_16_BIT_IF_FALSE2:
        if(CPUReadHalfWord(cheatsList[i].address) == cheatsList[i].value) {
          i+=2;
        }
        break;
      case GSA_32_BIT_IF_FALSE2:
        if(CPUReadMemory(cheatsList[i].address) == cheatsList[i].value) {
          i+=2;
        }
        break;
      case CBA_ADD:
        if ((cheatsList[i].address & 1) == 0) { 
          CPUWriteHalfWord(cheatsList[i].address, 
                           CPUReadHalfWord(cheatsList[i].address) +
                           cheatsList[i].value);
        } else {
          CPUWriteMemory(cheatsList[i].address & 0x0FFFFFFE, 
                           CPUReadMemory(cheatsList[i].address & 0x0FFFFFFE) +
                           cheatsList[i].value);
        }
        break;
      case CBA_OR:
        CPUWriteHalfWord(cheatsList[i].address,
                         CPUReadHalfWord(cheatsList[i].address) |
                         cheatsList[i].value);
        break;
      case CBA_GT:
        if (!(CPUReadHalfWord(cheatsList[i].address) > cheatsList[i].value)){
          i++;
        }
        break;
      case CBA_LT:
        if (!(CPUReadHalfWord(cheatsList[i].address) < cheatsList[i].value)){
          i++;
        }
        break;
      case CBA_SUPER:
		{
          int count = 2*((cheatsList[i].value -1) & 0xFFFF)+1;
          u32 address = cheatsList[i].address;
          for(int x = 0; x <= count; x++) {
            u8 b;
            int res = x % 6;
		    if (res==0)
		 	  i++;
            if(res < 4)
              b = (cheatsList[i].address >> (24-8*res)) & 0xFF;
            else
              b = (cheatsList[i].value >> (8 - 8*(res-4))) & 0xFF;
            CPUWriteByte(address, b);
            address++;
		  }
		}
        break;
      case GSA_8_BIT_POINTER :
        if ( 
			((CPUReadMemory(cheatsList[i].address)>=0x02000000) && (CPUReadMemory(cheatsList[i].address) < 0x02040000)) 
			||
            ((CPUReadMemory(cheatsList[i].address)>=0x03000000) && (CPUReadMemory(cheatsList[i].address)<0x03008000))
		)
        {
          CPUWriteByte(CPUReadMemory(cheatsList[i].address)+((cheatsList[i].value & 0xFFFFFF00) >> 8),
                       cheatsList[i].value & 0xFF);
        }
        break;
      case GSA_16_BIT_POINTER :
        if ( 
			((CPUReadMemory(cheatsList[i].address)>=0x02000000) && (CPUReadMemory(cheatsList[i].address) < 0x02040000)) 
			||
            ((CPUReadMemory(cheatsList[i].address)>=0x03000000) && (CPUReadMemory(cheatsList[i].address)<0x03008000))
		)
		{
          CPUWriteHalfWord(CPUReadMemory(cheatsList[i].address)+((cheatsList[i].value & 0xFFFF0000) >> 15),
                       cheatsList[i].value & 0xFFFF);
        }
        break;
      case GSA_32_BIT_POINTER :
        if ( 
			((CPUReadMemory(cheatsList[i].address)>=0x02000000) && (CPUReadMemory(cheatsList[i].address) < 0x02040000)) 
			||
            ((CPUReadMemory(cheatsList[i].address)>=0x03000000) && (CPUReadMemory(cheatsList[i].address)<0x03008000))
		)
		{
          CPUWriteMemory(CPUReadMemory(cheatsList[i].address),
                       cheatsList[i].value);
        }
        break;
      case GSA_8_BIT_ADD :
        CPUWriteByte(cheatsList[i].address,
						((cheatsList[i].value & 0xFF) + (CPUReadMemory(cheatsList[i].address) & 0xFF)) 
					);
        break;
      case GSA_16_BIT_ADD :
        CPUWriteHalfWord(cheatsList[i].address,
							((cheatsList[i].value & 0xFFFF) + (CPUReadMemory(cheatsList[i].address) & 0xFFFF))
						);
        break;
      case GSA_32_BIT_ADD :
        CPUWriteMemory(cheatsList[i].address ,
                       (cheatsList[i].value + (CPUReadMemory(cheatsList[i].address) & 0xFFFFFFFF) )
					   );
        break;
      case GSA_8_BIT_IF_LOWER_U:
        if (!(CPUReadByte(cheatsList[i].address) < (cheatsList[i].value & 0xFF))) {
          i++;
        }
        break;
      case GSA_16_BIT_IF_LOWER_U:
        if (!(CPUReadHalfWord(cheatsList[i].address) < (cheatsList[i].value & 0xFFFF))) {
          i++;
        }
        break;
      case GSA_32_BIT_IF_LOWER_U:
        if (!(CPUReadMemory(cheatsList[i].address) < cheatsList[i].value)) {
          i++;
        }
        break;
      case GSA_8_BIT_IF_HIGHER_U:
        if (!(CPUReadByte(cheatsList[i].address) > (cheatsList[i].value & 0xFF))) {
          i++;
        }
        break;
      case GSA_16_BIT_IF_HIGHER_U:
        if (!(CPUReadHalfWord(cheatsList[i].address) > (cheatsList[i].value & 0xFFFF))) {
          i++;
        }
        break;
      case GSA_32_BIT_IF_HIGHER_U:
        if (!(CPUReadMemory(cheatsList[i].address) > cheatsList[i].value)) {
          i++;
        }
        break;
      case GSA_8_BIT_IF_AND:
        if (!(CPUReadByte(cheatsList[i].address) & (cheatsList[i].value & 0xFF))) {
          i++;
        }
        break;
      case GSA_16_BIT_IF_AND:
        if (!(CPUReadHalfWord(cheatsList[i].address) & (cheatsList[i].value & 0xFFFF))) {
          i++;
        }
        break;
      case GSA_32_BIT_IF_AND:
        if (!(CPUReadMemory(cheatsList[i].address) & cheatsList[i].value)) {
          i++;
        }
        break;
      case GSA_8_BIT_IF_LOWER_U2:
        if (!(CPUReadByte(cheatsList[i].address) < (cheatsList[i].value & 0xFF))) {
          i+=2;
        }
        break;
      case GSA_16_BIT_IF_LOWER_U2:
        if (!(CPUReadHalfWord(cheatsList[i].address) < (cheatsList[i].value & 0xFFFF))) {
          i+=2;
        }
        break;
      case GSA_32_BIT_IF_LOWER_U2:
        if (!(CPUReadMemory(cheatsList[i].address) < cheatsList[i].value)) {
          i+=2;
        }
        break;
      case GSA_8_BIT_IF_HIGHER_U2:
        if (!(CPUReadByte(cheatsList[i].address) > (cheatsList[i].value & 0xFF))) {
          i+=2;
        }
        break;
      case GSA_16_BIT_IF_HIGHER_U2:
        if (!(CPUReadHalfWord(cheatsList[i].address) > (cheatsList[i].value & 0xFFFF))) {
          i+=2;
        }
        break;
      case GSA_32_BIT_IF_HIGHER_U2:
        if (!(CPUReadMemory(cheatsList[i].address) > cheatsList[i].value)) {
          i+=2;
        }
        break;
      case GSA_8_BIT_IF_AND2:
        if (!(CPUReadByte(cheatsList[i].address) & (cheatsList[i].value & 0xFF))) {
          i+=2;
        }
        break;
      case GSA_16_BIT_IF_AND2:
        if (!(CPUReadHalfWord(cheatsList[i].address) & (cheatsList[i].value & 0xFFFF))) {
          i+=2;
        }
        break;
      case GSA_32_BIT_IF_AND2:
        if (!(CPUReadMemory(cheatsList[i].address) & cheatsList[i].value)) {
          i+=2;
        }
        break;
      case GSA_ALWAYS:
        i++;
        break;
      case GSA_ALWAYS2:
        i+=2;
        break;
      case GSA_8_BIT_IF_LOWER_S:
        if (!((s8)CPUReadByte(cheatsList[i].address) < ((s8)cheatsList[i].value & 0xFF))) {
          i++;
        }
        break;
      case GSA_16_BIT_IF_LOWER_S:
        if (!((s16)CPUReadHalfWord(cheatsList[i].address) < ((s16)cheatsList[i].value & 0xFFFF))) {
          i++;
        }
        break;
      case GSA_32_BIT_IF_LOWER_S:
        if (!((s32)CPUReadMemory(cheatsList[i].address) < (s32)cheatsList[i].value)) {
          i++;
        }
        break;
      case GSA_8_BIT_IF_HIGHER_S:
        if (!((s8)CPUReadByte(cheatsList[i].address) > ((s8)cheatsList[i].value & 0xFF))) {
          i++;
        }
        break;
      case GSA_16_BIT_IF_HIGHER_S:
        if (!((s16)CPUReadHalfWord(cheatsList[i].address) > ((s16)cheatsList[i].value & 0xFFFF))) {
          i++;
        }
        break;
      case GSA_32_BIT_IF_HIGHER_S:
        if (!((s32)CPUReadMemory(cheatsList[i].address) > (s32)cheatsList[i].value)) {
          i++;
        }
        break;
      case GSA_8_BIT_IF_LOWER_S2:
        if (!((s8)CPUReadByte(cheatsList[i].address) < ((s8)cheatsList[i].value & 0xFF))) {
          i+=2;
        }
        break;
      case GSA_16_BIT_IF_LOWER_S2:
        if (!((s16)CPUReadHalfWord(cheatsList[i].address) < ((s16)cheatsList[i].value & 0xFFFF))) {
          i+=2;
        }
        break;
      case GSA_32_BIT_IF_LOWER_S2:
        if (!((s32)CPUReadMemory(cheatsList[i].address) < (s32)cheatsList[i].value)) {
          i+=2;
        }
        break;
      case GSA_8_BIT_IF_HIGHER_S2:
        if (!((s8)CPUReadByte(cheatsList[i].address) > ((s8)cheatsList[i].value & 0xFF))) {
          i+=2;
        }
        break;
      case GSA_16_BIT_IF_HIGHER_S2:
        if (!((s16)CPUReadHalfWord(cheatsList[i].address) > ((s16)cheatsList[i].value & 0xFFFF))) {
          i+=2;
        }
        break;
      case GSA_32_BIT_IF_HIGHER_S2:
        if (!((s32)CPUReadMemory(cheatsList[i].address) > (s32)cheatsList[i].value)) {
          i+=2;
        }
        break;
      case GSA_16_BIT_WRITE_IOREGS:
        if ((cheatsList[i].address <= 0x3FF) && (cheatsList[i].address != 0x6) &&
            (cheatsList[i].address != 0x130)) 
          ioMem[cheatsList[i].address & 0x3FE]=cheatsList[i].value & 0xFFFF;
        break;
      case GSA_32_BIT_WRITE_IOREGS:
        if (cheatsList[i].address<=0x3FF)  
        {
          if (((cheatsList[i].address & 0x3FC) != 0x6) && ((cheatsList[i].address & 0x3FC) != 0x130))
            ioMem[cheatsList[i].address & 0x3FC]= (cheatsList[i].value & 0xFFFF);
          if ((((cheatsList[i].address & 0x3FC)+2) != 0x6) && ((cheatsList[i].address & 0x3FC) +2) != 0x130)
            ioMem[(cheatsList[i].address & 0x3FC) + 2 ]= ((cheatsList[i].value>>16 ) & 0xFFFF);
        }
        break;
      case GSA_8_BIT_IF_TRUE3:
        if(CPUReadByte(cheatsList[i].address) != cheatsList[i].value) {
          onoff=false;
        }
        break;
      case GSA_16_BIT_IF_TRUE3:
        if(CPUReadHalfWord(cheatsList[i].address) != cheatsList[i].value) {
          onoff=false;
        }
        break;
      case GSA_32_BIT_IF_TRUE3:
        if(CPUReadMemory(cheatsList[i].address) != cheatsList[i].value) {
          onoff=false;
        }
        break;
      case GSA_8_BIT_IF_FALSE3:
        if(CPUReadByte(cheatsList[i].address) == cheatsList[i].value) {
          onoff=false;
        }
        break;
      case GSA_16_BIT_IF_FALSE3:
        if(CPUReadHalfWord(cheatsList[i].address) == cheatsList[i].value) {
          onoff=false;
        }
        break;
      case GSA_32_BIT_IF_FALSE3:
        if(CPUReadMemory(cheatsList[i].address) == cheatsList[i].value) {
          onoff=false;
        }
        break;
      case GSA_8_BIT_IF_LOWER_S3:
        if (!((s8)CPUReadByte(cheatsList[i].address) < ((s8)cheatsList[i].value & 0xFF))) {
          onoff=false;
        }
        break;
      case GSA_16_BIT_IF_LOWER_S3:
        if (!((s16)CPUReadHalfWord(cheatsList[i].address) < ((s16)cheatsList[i].value & 0xFFFF))) {
          onoff=false;
        }
        break;
      case GSA_32_BIT_IF_LOWER_S3:
        if (!((s32)CPUReadMemory(cheatsList[i].address) < (s32)cheatsList[i].value)) {
          onoff=false;
        }
        break;
      case GSA_8_BIT_IF_HIGHER_S3:
        if (!((s8)CPUReadByte(cheatsList[i].address) > ((s8)cheatsList[i].value & 0xFF))) {
          onoff=false;
        }
        break;
      case GSA_16_BIT_IF_HIGHER_S3:
        if (!((s16)CPUReadHalfWord(cheatsList[i].address) > ((s16)cheatsList[i].value & 0xFFFF))) {
          onoff=false;
        }
        break;
      case GSA_32_BIT_IF_HIGHER_S3:
        if (!((s32)CPUReadMemory(cheatsList[i].address) > (s32)cheatsList[i].value)) {
          onoff=false;
        }
        break;
      case GSA_8_BIT_IF_LOWER_U3:
        if (!(CPUReadByte(cheatsList[i].address) < (cheatsList[i].value & 0xFF))) {
          onoff=false;
        }
        break;
      case GSA_16_BIT_IF_LOWER_U3:
        if (!(CPUReadHalfWord(cheatsList[i].address) < (cheatsList[i].value & 0xFFFF))) {
          onoff=false;
        }
        break;
      case GSA_32_BIT_IF_LOWER_U3:
        if (!(CPUReadMemory(cheatsList[i].address) < cheatsList[i].value)) {
          onoff=false;
        }
        break;
      case GSA_8_BIT_IF_HIGHER_U3:
        if (!(CPUReadByte(cheatsList[i].address) > (cheatsList[i].value & 0xFF))) {
          onoff=false;
        }
        break;
      case GSA_16_BIT_IF_HIGHER_U3:
        if (!(CPUReadHalfWord(cheatsList[i].address) > (cheatsList[i].value & 0xFFFF))) {
          onoff=false;
        }
        break;
      case GSA_32_BIT_IF_HIGHER_U3:
        if (!(CPUReadMemory(cheatsList[i].address) > cheatsList[i].value)) {
          onoff=false;
        }
        break;
      case GSA_8_BIT_IF_AND3:
        if (!(CPUReadByte(cheatsList[i].address) & (cheatsList[i].value & 0xFF))) {
          onoff=false;
        }
        break;
      case GSA_16_BIT_IF_AND3:
        if (!(CPUReadHalfWord(cheatsList[i].address) & (cheatsList[i].value & 0xFFFF))) {
          onoff=false;
        }
        break;
      case GSA_32_BIT_IF_AND3:
        if (!(CPUReadMemory(cheatsList[i].address) & cheatsList[i].value)) {
          onoff=false;
        }
        break;
      case GSA_ALWAYS3:
        if (!(CPUReadMemory(cheatsList[i].address) & cheatsList[i].value)) {
          onoff=false;
        }
        break;
      case GSA_GROUP_WRITE:
      	{
          int count = ((cheatsList[i].address) & 0xFFFE) +1;
          u32 value = cheatsList[i].value;
		  if (count==0)
			  i++;
		  else
            for (int x = 1; x <= count; x++) {
				if ((x % 2) ==0){
					if (x<count)
						i++;
					CPUWriteMemory(cheatsList[i].rawaddress, value);
				}
				else
					CPUWriteMemory(cheatsList[i].value, value);
			}
		}
		break;
      case GSA_32_BIT_ADD2:
        CPUWriteMemory(cheatsList[i].value ,
                       (CPUReadMemory(cheatsList[i].value) + cheatsList[i+1].rawaddress) & 0xFFFFFFFF);
        i++;
		break;
      case GSA_32_BIT_SUB2:
        CPUWriteMemory(cheatsList[i].value ,
                       (CPUReadMemory(cheatsList[i].value) - cheatsList[i+1].rawaddress) & 0xFFFFFFFF);
        i++;
		break;
      case GSA_16_BIT_IF_LOWER_OR_EQ_U:
        if(CPUReadHalfWord(cheatsList[i].address) > cheatsList[i].value) {
          i++;
        }
        break;
      case GSA_16_BIT_IF_HIGHER_OR_EQ_U:
        if(CPUReadHalfWord(cheatsList[i].address) < cheatsList[i].value) {
          i++;
        }
        break;
      case GSA_16_BIT_MIF_TRUE:
        if(CPUReadHalfWord(cheatsList[i].address) != cheatsList[i].value) {
          i+=((cheatsList[i].rawaddress >> 0x10) & 0xFF);
        }
        break;
      case GSA_16_BIT_MIF_FALSE:
        if(CPUReadHalfWord(cheatsList[i].address) == cheatsList[i].value) {
          i+=(cheatsList[i].rawaddress >> 0x10) & 0xFF;
        }
        break;
      case GSA_16_BIT_MIF_LOWER_OR_EQ_U:
        if(CPUReadHalfWord(cheatsList[i].address) > cheatsList[i].value) {
          i+=(cheatsList[i].rawaddress >> 0x10) & 0xFF;
        }
        break;
      case GSA_16_BIT_MIF_HIGHER_OR_EQ_U:
        if(CPUReadHalfWord(cheatsList[i].address) < cheatsList[i].value) {
          i+=(cheatsList[i].rawaddress >> 0x10) & 0xFF;
        }
        break;
      case CHEATS_16_BIT_WRITE:
        if ((cheatsList[i].address>>24)>=0x08) {
          CHEAT_PATCH_ROM_16BIT(cheatsList[i].address, cheatsList[i].value);
        } else {
          CPUWriteHalfWord(cheatsList[i].address, cheatsList[i].value);
        }
        break;
      case CHEATS_32_BIT_WRITE:
        if ((cheatsList[i].address>>24)>=0x08) {
          CHEAT_PATCH_ROM_32BIT(cheatsList[i].address, cheatsList[i].value);
        } else {
          CPUWriteMemory(cheatsList[i].address, cheatsList[i].value);
        }
        break;
      }
    }
  }
  for (i = 0; i<4; i++)
    if (rompatch2addr [i] != 0)
      CHEAT_PATCH_ROM_16BIT(rompatch2addr [i],rompatch2val [i]);
  return ticks;
}
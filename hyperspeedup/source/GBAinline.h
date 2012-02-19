// -*- C++ -*-
// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2005 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef VBA_GBAinline_H
#define VBA_GBAinline_H

#include "System.h"
#include "Port.h"
#include "RTC.h"
#include "GBA.h"

#include "ichflysettings.h"

#include <nds/interrupts.h>

extern bool cpuSramEnabled;
extern bool cpuFlashEnabled;
extern bool cpuEEPROMEnabled;
extern bool cpuEEPROMSensorEnabled;
extern bool cpuDmaHack;
extern u32 cpuDmaLast;
extern bool timer0On;
extern int timer0Ticks;
extern int timer0ClockReload;
extern bool timer1On;
extern int timer1Ticks;
extern int timer1ClockReload;
extern bool timer2On;
extern int timer2Ticks;
extern int timer2ClockReload;
extern bool timer3On;
extern int timer3Ticks;
extern int timer3ClockReload;
extern int cpuTotalTicks;

void Logsd(const char *defaultMsg,...);


#define UPDATE_REG(address, value)\
  {\
    WRITE16LE(((u16 *)&ioMem[address]),value);\
  }\

/*
u8 CPUReadByteQuick(u32 addr);
u16 CPUReadHalfWordQuick(u32 addr);
u32 CPUReadMemoryQuick(u32 addr);
*/

#define CPUReadByteQuick(addr) \
  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]

#define CPUReadHalfWordQuick(addr) \
  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define CPUReadMemoryQuick(addr) \
  READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))


static u32 CPUReadMemoryreal(u32 address);

__attribute__((section(".itcm"))) static inline u32 CPUReadMemory(u32 address)
 {
	return CPUReadMemoryreal(address);
 }

static u32 CPUReadHalfWordreal(u32 address);

__attribute__((section(".itcm"))) static inline u32 CPUReadHalfWord(u32 address)
 {
	return CPUReadHalfWordreal(address);
 }
static u8 CPUReadBytereal(u32 address);

__attribute__((section(".itcm"))) static inline u8 CPUReadByte(u32 address)
 {
	return CPUReadBytereal(address);
 }


__attribute__((section(".itcm"))) static inline void updateVC()
{
		u16 temp = REG_VCOUNT;
		u16 temp2 = REG_DISPSTAT;
		//iprintf("Vcountreal: %08x\n",temp);
		float help = temp;
		u16 help3;
		if(temp < 192)
		{
			VCOUNT = help * (1./1.2); //1.15350877;
			//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
		}
		else
		{
			VCOUNT = ((help - 192) * (1./ 1.04411764)) + 160; //1.15350877;
			//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
		}
		DISPSTAT &= 0xFFFC; //reset h-blanc and V-Blanc
		//if(help3 == VCOUNT) //else it is a extra long V-Line // ichfly todo it is to slow
		//{
			DISPSTAT |= (temp2 & 0x2); //temporary patch
		//}
		if(VCOUNT > 160 && VCOUNT != 227)DISPSTAT |= 1;//V-Blanc
		UPDATE_REG(0x06, VCOUNT);
		if(VCOUNT == (DISPSTAT >> 8)) //update by V-Count Setting
		{
			DISPSTAT |= 4;
			if(DISPSTAT & 0x20) {
			  IF |= 4;
			  UPDATE_REG(0x202, IF);
			}
		  } else {
			DISPSTAT &= 0xFFFB;
		}
		UPDATE_REG(0x04, DISPSTAT);
		//iprintf("Vcountreal: %08x\n",temp);
		//iprintf("DISPSTAT: %08x\n",temp2);
}


 static inline u32 CPUReadMemoryreal(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef DEV_VERSION
  if(address & 3) {  
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      log("Unaligned word read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
#ifdef printreads
  iprintf("r32 %08x\n",address);
#endif
  
  u32 value;
  switch(address >> 24) {
  case 0:
    if(reg[15].I >> 24) {
      if(address < 0x4000) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_ILLEGAL_READ) {
          log("Illegal word read: %08x at %08x\n", address, armMode ?
              armNextPC - 4 : armNextPC - 2);
        }
#endif
        
        value = READ32LE(((u32 *)&biosProtected));
      }
      else goto unreadable;
    } else
      value = READ32LE(((u32 *)&bios[address & 0x3FFC]));
    break;
  case 2:
    value = READ32LE(((u32 *)&workRAM[address & 0x3FFFC]));
    break;
  case 3:
    value = READ32LE(((u32 *)&internalRAM[address & 0x7ffC]));
    break;
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		//todo timer shift
		value = *(u32 *)(address);
		break;
	}
	
	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		updateVC();
	}

	if((address & ~0x3) == 0x4000200)//ichfly update
	{
		IF = (REG_IF & 0x3FFF & ~(0x1 & anytimejmpfilter)); //VBlanc
		UPDATE_REG(0x202, IF);
	}
    if((address < 0x4000400) && ioReadable[address & 0x3fc]) {
      if(ioReadable[(address & 0x3fc) + 2])
        value = READ32LE(((u32 *)&ioMem[address & 0x3fC]));
      else
        value = READ16LE(((u16 *)&ioMem[address & 0x3fc]));
    } else goto unreadable;
    break;
  case 5:
    value = READ32LE(((u32 *)&paletteRAM[address & 0x3fC]));
    break;
  case 6:
    address = (address & 0x1fffc);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
    {
        value = 0;
        break;
    }
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    value = READ32LE(((u32 *)&vram[address]));
    break;
  case 7:
    value = READ32LE(((u32 *)&oam[address & 0x3FC]));
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
#ifdef uppern_read_emulation
	if((address&0x1FFFFFC) > romSize)
	{
		//iprintf("high word read");
		fseek (ichflyfilestream , address&0x1FFFFFC , SEEK_SET);
		fread (&value,1,4,ichflyfilestream);
	}
	else
	{
		value = READ32LE(((u32 *)&rom[address&0x1FFFFFC]));
	}
#else
    value = READ32LE(((u32 *)&rom[address&0x1FFFFFC]));
#endif
    break;
  case 13:
    if(cpuEEPROMEnabled)
      // no need to swap this
      return eepromRead(address);
    goto unreadable;
  case 14:
    if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
  //while(1);
#ifdef DEV_VERSION

      log("Illegal word read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
#endif

    if(cpuDmaHack) {
      value = cpuDmaLast;
    } else {
      if(armState) {
        value = CPUReadMemoryQuick(reg[15].I);
      } else {
        value = CPUReadHalfWordQuick(reg[15].I) |
          CPUReadHalfWordQuick(reg[15].I) << 16;
      }
    }
  }

  if(address & 3) {
//#ifdef C_CORE
    int shift = (address & 3) << 3;
    value = (value >> shift) | (value << (32 - shift));
/*#else    
#ifdef __GNUC__ ichfly
    asm("and $3, %%ecx;"
        "shl $3 ,%%ecx;"
        "ror %%cl, %0"
        : "=r" (value)
        : "r" (value), "c" (address));
#else
    asm(
      mov ecx, address;
      and ecx, 3;
      shl ecx, 3;
      ror [dword ptr value], cl;
    )
//#endif
#endif*/
  }
  return value;
}

extern u32 myROM[];

static inline u32 CPUReadHalfWordreal(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
	iprintf("r16 %04x\n",address);
#endif
#ifdef DEV_VERSION      
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      log("Unaligned halfword read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  u32 value;
  
  switch(address >> 24) {
  case 0:
    if (reg[15].I >> 24) {
      if(address < 0x4000) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_ILLEGAL_READ) {
          log("Illegal halfword read: %08x at %08x\n", address, armMode ?
              armNextPC - 4 : armNextPC - 2);
        }
#endif
        value = READ16LE(((u16 *)&biosProtected[address&2]));
      } else goto unreadable;
    } else
      value = READ16LE(((u16 *)&bios[address & 0x3FFE]));
    break;
  case 2:
    value = READ16LE(((u16 *)&workRAM[address & 0x3FFFE]));
    break;
  case 3:
    value = READ16LE(((u16 *)&internalRAM[address & 0x7ffe]));
    break;
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		
		if((address&0x2) == 0)
		{
			if(ioMem[address & 0x3fe] & 0x8000)
			{
				/*if((ioMem[(address & 0x3fe) + 0x2] & 0x3) == 0)
				{
					value = (*(u16 *)(address)) << 5;
				}
				if((ioMem[(address & 0x3fe) + 0x2] & 0x3) == 1)
				{
					value = (*(u16 *)(address)) << 1;
				}
				if((ioMem[(address & 0x3fe) + 0x2] & 0x3) == 2)
				{
					value = (*(u16 *)(address)) << 1;
				}
				if((ioMem[(address & 0x3fe) + 0x2] & 0x3) == 3)
				{
					value = *(u16 *)(address); //todo timer shift
				}*/
				value = ((*(u16 *)(address)) >> 1) | 0x8000;
			}
			else
			{
				value = (*(u16 *)(address)) >> 1;
			}
			return value;
		}
	}
  
	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		updateVC();
	}
	
	if(address == 0x4000202)//ichfly update
	{
		IF = (REG_IF & 0x3FFF & ~(0x1 & anytimejmpfilter)); //VBlanc
		UPDATE_REG(0x202, IF);
	}
	
    if((address < 0x4000400) && ioReadable[address & 0x3fe])
    {
		value =  READ16LE(((u16 *)&ioMem[address & 0x3fe]));
    }
    else goto unreadable;
    break;
  case 5:
    value = READ16LE(((u16 *)&paletteRAM[address & 0x3fe]));
    break;
  case 6:
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
    {
        value = 0;
        break;
    }
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    value = READ16LE(((u16 *)&vram[address]));
    break;
  case 7:
    value = READ16LE(((u16 *)&oam[address & 0x3fe]));
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
    if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8)
      value = rtcRead(address);
    else
	{
#ifdef uppern_read_emulation
	if((address&0x1FFFFFE) > romSize)
	{
		//iprintf("high hword read");
		fseek (ichflyfilestream , address&0x1FFFFFE , SEEK_SET);
		fread (&value,1,2,ichflyfilestream);
	}
	else
	{
		value = READ16LE(((u16 *)&rom[address & 0x1FFFFFE]));
	}
#else
    value = READ16LE(((u16 *)&rom[address & 0x1FFFFFE]));
#endif
	}
    break;    
  case 13:
    if(cpuEEPROMEnabled)
      // no need to swap this
      return  eepromRead(address);
    goto unreadable;
  case 14:
    if(cpuFlashEnabled | cpuSramEnabled)
      // no need to swap this
      return flashRead(address);
    // default
  default:
  unreadable:
#ifdef DEV_VERSION
    if(systemVerbose & VERBOSE_ILLEGAL_READ) {
      log("Illegal halfword read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
#endif
    if(cpuDmaHack) {
      value = cpuDmaLast & 0xFFFF;
    } else {
      if(armState) {
        value = CPUReadHalfWordQuick(reg[15].I + (address & 2));
      } else {
        value = CPUReadHalfWordQuick(reg[15].I);
      }
    }
    break;
  }

  if(address & 1) {
    value = (value >> 8) | (value << 24);
  }
  
  return value;
}

static inline u16 CPUReadHalfWordSigned(u32 address)
{
  u16 value = CPUReadHalfWord(address);
  if((address & 1))
    value = (s8)value;
  return value;
}

static inline u8 CPUReadBytereal(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
iprintf("r8 %02x\n",address);
#endif

  switch(address >> 24) {
  case 0:
    if (reg[15].I >> 24) {
      if(address < 0x4000) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_ILLEGAL_READ) {
          log("Illegal byte read: %08x at %08x\n", address, armMode ?
              armNextPC - 4 : armNextPC - 2);
        }
#endif
        return biosProtected[address & 3];
      } else goto unreadable;
    }
    return bios[address & 0x3FFF];
  case 2:
    return workRAM[address & 0x3FFFF];
  case 3:
    return internalRAM[address & 0x7fff];
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		//todo timer shift
		return *(u8 *)(address);
	}
  
  	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		updateVC();
	}
	if(address == 0x4000202 || address == 0x4000203)//ichfly update
	{
		IF = (REG_IF & 0x3FFF & ~(0x1 & anytimejmpfilter)); //VBlanc
		UPDATE_REG(0x202, IF);
	}
    if((address < 0x4000400) && ioReadable[address & 0x3ff])
      return ioMem[address & 0x3ff];
    else goto unreadable;
  case 5:
    return paletteRAM[address & 0x3ff];
  case 6:
    address = (address & 0x1ffff);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return 0;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
    return vram[address];
  case 7:
    return oam[address & 0x3ff];
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:

#ifdef uppern_read_emulation
	if((address&0x1FFFFFF) > romSize)
	{
		//iprintf("high byte read %08X %08X\r\n", address,romSize);
		u8 temp = 0;
		fseek (ichflyfilestream , address&0x1FFFFFF , SEEK_SET);
		fread (&temp,1,1,ichflyfilestream);
		return temp;
	}
	else
	{
		return rom[address & 0x1FFFFFF];
	}
#else
    return rom[address & 0x1FFFFFF];
#endif        
  case 13:
    if(cpuEEPROMEnabled)
      return eepromRead(address);
    goto unreadable;
  case 14:
    if(cpuSramEnabled | cpuFlashEnabled)
      return flashRead(address);
    if(cpuEEPROMSensorEnabled) {
      switch(address & 0x00008f00) {
      case 0x8200:
        return systemGetSensorX() & 255;
      case 0x8300:
        return (systemGetSensorX() >> 8)|0x80;
      case 0x8400:
        return systemGetSensorY() & 255;
      case 0x8500:
        return systemGetSensorY() >> 8;
      }
    }
    // default
  default:
  unreadable:
#ifdef DEV_VERSION
    if(systemVerbose & VERBOSE_ILLEGAL_READ) {
      log("Illegal byte read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
#endif
    if(cpuDmaHack) {
      return cpuDmaLast & 0xFF;
    } else {
      if(armState) {
        return CPUReadByteQuick(reg[15].I+(address & 3));
      } else {
        return CPUReadByteQuick(reg[15].I+(address & 1));
      }
    }
    break;
  }
}

static inline void CPUWriteMemory(u32 address, u32 value) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
    iprintf("w32 %08x to %08x\n",value,address);
#endif		  
		

#ifdef DEV_VERSION
  if(address & 3) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      log("Unaligned word write: %08x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  switch(address >> 24) {
  case 0x02:
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeWorkRAM[address & 0x3FFFC]))
      cheatsWriteMemory(address & 0x203FFFC,
                        value);
    else
#endif
      WRITE32LE(((u32 *)&workRAM[address & 0x3FFFC]), value);
    break;
  case 0x03:
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeInternalRAM[address & 0x7ffc]))
      cheatsWriteMemory(address & 0x3007FFC,
                        value);
    else
#endif
      WRITE32LE(((u32 *)&internalRAM[address & 0x7ffC]), value);
    break;
  case 0x04:
    if(address < 0x4000400) {

	/*if((0x4000060 > address && address > 0x4000007) || (address > 0x40000FF && address < 0x4000110)) //timer and lcd
	{
			//iprintf("32 %x %x\r\n",address,value);
		    *(u32 *)(address) = value;
	}
	else //dont do twice*/ //don't need that any more
	{
      CPUUpdateRegister((address & 0x3FC), value & 0xFFFF);
      CPUUpdateRegister((address & 0x3FC) + 2, (value >> 16));
    }
	} else goto unwritable;
    break;
  case 0x05:
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezePRAM[address & 0x3fc]))
      cheatsWriteMemory(address & 0x70003FC,
                        value);
    else
#endif
    WRITE32LE(((u32 *)&paletteRAM[address & 0x3FC]), value);
    break;
  case 0x06:
    address = (address & 0x1fffc);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeVRAM[address]))
      cheatsWriteMemory(address + 0x06000000, value);
    else
#endif
    
    WRITE32LE(((u32 *)&vram[address]), value);
    break;
  case 0x07:
#ifdef BKPT_SUPPORT
    if(*((u32 *)&freezeOAM[address & 0x3fc]))
      cheatsWriteMemory(address & 0x70003FC,
                        value);
    else
#endif
    WRITE32LE(((u32 *)&oam[address & 0x3fc]), value);
    break;
  case 0x0D:
    if(cpuEEPROMEnabled) {
      eepromWrite(address, value);
      break;
    }
    goto unwritable;
  case 0x0E:
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    // default
  default:
  unwritable:
#ifdef DEV_VERSION
    if(systemVerbose & VERBOSE_ILLEGAL_WRITE) {
      log("Illegal word write: %08x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
#endif
    break;
  }
}

#endif //VBA_GBAinline_H

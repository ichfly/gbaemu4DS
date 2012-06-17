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

#include "agbprint.h"
#include "System.h"
#include "Port.h"
#include "RTC.h"
#include "GBA.h"

#include "ichflysettings.h"
#include "cpumg.h"
#include "main.h"

#include <nds/interrupts.h>

const u32  objTilesAddress [3] = {0x010000, 0x014000, 0x014000};


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


void updateVCsub()
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
			/*if(DISPSTAT & 0x20) {
			  IF |= 4;
			  UPDATE_REG(0x202, IF);
			}*/
		  } else {
			DISPSTAT &= 0xFFFB;
		}
		UPDATE_REG(0x04, DISPSTAT);
		//iprintf("Vcountreal: %08x\n",temp);
		//iprintf("DISPSTAT: %08x\n",temp2);
}


inline u32 CPUReadMemoryrealpu(u32 address)
{
#ifdef DEV_VERSION
  if(address & 3) {  
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned word read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
#ifdef printreads
  iprintf("r32 %08x\n",address);
#endif
  
  u32 value;
  switch(address >> 24) {
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		//todo timer shift
		value = *(u32 *)(address);
		break;
	}
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202 || address == 0x4000200)//ichfly update
	{
		IF = (REG_IF & 0x3FFF); //VBlanc
		UPDATE_REG(0x202, IF);
	}
#endif	


	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		updateVCsub();
	}
    if((address < 0x4000400) && ioReadable[address & 0x3fc]) {
      if(ioReadable[(address & 0x3fc) + 2])
        value = READ32LE(((u32 *)&ioMem[address & 0x3fC]));
      else
        value = READ16LE(((u16 *)&ioMem[address & 0x3fc]));
    } else goto unreadable;
    break;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
#ifdef uppern_read_emulation
	if((address&0x1FFFFFC) > romSize)
	{
#ifdef print_uppern_read_emulation
		iprintf("high r32 %08x\n",address);
#endif
		fseek(ichflyfilestream , address&0x1FFFFFC , SEEK_SET);
		fread(&value,1,4,ichflyfilestream);
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
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  Log("Illegal word read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    /*if(cpuDmaHack) { //ichly won't work
      value = cpuDmaLast;
    } else {
      if(armState) {
        value = CPUReadMemoryQuick(reg[15].I);
      } else {
        value = CPUReadHalfWordQuick(reg[15].I) |
          CPUReadHalfWordQuick(reg[15].I) << 16;
      }
    }*/
	  break;
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

inline u32 CPUReadHalfWordrealpu(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
	iprintf("r16 %08x\n",address);
#endif
#ifdef DEV_VERSION      
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned halfword read: %08x at %08x\n", address, armMode ?
          armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  u32 value;
  
  switch(address >> 24) {
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		
		if((address&0x2) == 0)
		{
			if(ioMem[address & 0x3fe] & 0x8000)
			{
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
		updateVCsub();
	}
	
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202)//ichfly update
	{
		IF = (REG_IF & 0x3FFF);
		UPDATE_REG(0x202, IF);
	}
#endif
	
    if((address < 0x4000400) && ioReadable[address & 0x3fe])
    {
		value =  READ16LE(((u16 *)&ioMem[address & 0x3fe]));
    }
    else goto unreadable;
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
#ifdef print_uppern_read_emulation
		iprintf("high r16 %08x\n",address);
#endif
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
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  Log("Illegal hword read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    /*if(cpuDmaHack) {
      value = cpuDmaLast & 0xFFFF;
    } else {
      if(armState) {
        value = CPUReadHalfWordQuick(reg[15].I + (address & 2));
      } else {
        value = CPUReadHalfWordQuick(reg[15].I);
      }
    }*/
    break;
  }

  if(address & 1) {
    value = (value >> 8) | (value << 24);
  }
  
  return value;
}

inline u8 CPUReadByterealpu(u32 address) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
iprintf("r8 %02x\n",address);
#endif

  switch(address >> 24) {
  case 4:
  
	if(address > 0x40000FF && address < 0x4000111)
	{
		//todo timer shift
		return *(u8 *)(address);
	}
  
  	if(address > 0x4000003 && address < 0x4000008)//ichfly update
	{
		updateVCsub();
	}
#ifdef gba_handel_IRQ_correct
	if(address == 0x4000202 || address == 0x4000203)//ichfly update
	{
		IF = (REG_IF & 0x3FFF); //VBlanc
		UPDATE_REG(0x202, IF);
	}
#endif
    if((address < 0x4000400) && ioReadable[address & 0x3ff])
      return ioMem[address & 0x3ff];
    else goto unreadable;
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:

#ifdef uppern_read_emulation
	if((address&0x1FFFFFF) > romSize)
	{
#ifdef print_uppern_read_emulation
		iprintf("high r8 %08x\n",address);
#endif
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
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  Log("Illegal byte read: %08x\n", address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif

    /*if(cpuDmaHack) {
      return cpuDmaLast & 0xFF;
    } else {
      if(armState) {
        return CPUReadByteQuick(reg[15].I+(address & 3));
      } else {
        return CPUReadByteQuick(reg[15].I+(address & 1));
      }
    }*/
    break;
  }
}

inline void CPUWriteMemorypu(u32 address, u32 value) //ichfly not inline is faster because it is smaler
{
#ifdef printreads
    iprintf("w32 %08x to %08x\n",value,address);
#endif		  
		

#ifdef DEV_VERSION
  if(address & 3) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned word write: %08x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  switch(address >> 24) {
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
 unreadable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  Log("Illegal word write: %08x to %08x\n",value, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}
inline void CPUWriteHalfWordpu(u32 address, u16 value)
{
#ifdef printreads
iprintf("w16 %04x to %08x\r\n",value,address);
#endif

#ifdef DEV_VERSION
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned halfword write: %04x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  switch(address >> 24) {  
  case 4:
    if(address < 0x4000400)
      CPUUpdateRegister(address & 0x3fe, value);
    else goto unwritable;
    break;
  case 8:
  case 9:
    if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8) {
      if(!rtcWrite(address, value))
        goto unwritable;
    } else if(!agbPrintWrite(address, value)) goto unwritable;
    break;
  case 13:
    if(cpuEEPROMEnabled) {
      eepromWrite(address, (u8)value);
      break;
    }
    goto unwritable;
  case 14:
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    goto unwritable;
  default:
  unwritable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  Log("Illegal hword write: %04x to %08x\n",value, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

inline void CPUWriteBytepu(u32 address, u8 b)
{
#ifdef printreads
	iprintf("w8 %02x to %08x\r\n",b,address);
#endif
  switch(address >> 24) {
  case 4:
    if(address < 0x4000400) {
      switch(address & 0x3FF) {
      case 0x301:
	/*if(b == 0x80) //todo
	  stopState = true;
	holdState = 1;
	holdType = -1;
  cpuNextEvent = cpuTotalTicks;
	break;*/
      case 0x60:
      case 0x61:
      case 0x62:
      case 0x63:
      case 0x64:
      case 0x65:
      case 0x68:
      case 0x69:
      case 0x6c:
      case 0x6d:
      case 0x70:
      case 0x71:
      case 0x72:
      case 0x73:
      case 0x74:
      case 0x75:
      case 0x78:
      case 0x79:
      case 0x7c:
      case 0x7d:
      case 0x80:
      case 0x81:
      case 0x84:
      case 0x85:
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
	//soundEvent(address&0xFF, b);  //ichfly disable sound
#ifdef printsoundwrites
		  iprintf("b %02x to %08x\r\n",b,address);
#endif
	  #ifdef arm9advsound
		  REG_IPC_FIFO_TX = (address | 0x40000000);
		  REG_IPC_FIFO_TX = (b | 0x80000000); //faster in case we send a 0
		#endif
	break;
      default:
	/*if((0x4000060 > address && address > 0x4000008) || (address > 0x40000FF && address < 0x4000110))
	{
			//iprintf("8 %x %x\r\n",address,b);
		    *(u8 *)(address) = b;
	}*/ //ichfly don't need that
	if(address & 1)
	{
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16 *)&ioMem[address & 0x3fe])))
			     & 0x00FF) |
			    b<<8);

	}
	else
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16 *)&ioMem[address & 0x3fe])) & 0xFF00) | b));
      }
      break;
    } else goto unwritable;
    break;   
  case 13:
    if(cpuEEPROMEnabled) {
      eepromWrite(address, b);
      break;
    }
    goto unwritable;
  case 14:
      if (!(saveType == 5) && (!eepromInUse | cpuSramEnabled | cpuFlashEnabled)) {

    //if(!cpuEEPROMEnabled && (cpuSramEnabled | cpuFlashEnabled)) { 

        (*cpuSaveGameFunc)(address, b);
      break;
    }
    // default
  default:
  unwritable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  Log("Illegal byte write: %02x to %08x\n",b, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}


inline u16 CPUReadHalfWordrealpuSigned(u32 address)
{
  u16 value = CPUReadHalfWordrealpu(address);
  if((address & 1))
    value = (s8)value;
  return value;
}
#endif //VBA_GBAinline_H
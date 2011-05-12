#include "main.h"


extern "C" void swiHalt(void);

#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdarg.h>
#include <string.h>

#include "GBA.h"
#include "GBAinline.h"
#include "Globals.h"
#include "Gfx.h"
#include "EEprom.h"
#include "Flash.h"
#include "Sound.h"
#include "Sram.h"
#include "bios.h"
#include "unzip.h"
#include "Cheats.h"
#include "NLS.h"
#include "elf.h"
#include "Util.h"
#include "Port.h"
#include "agbprint.h"


void CPUWriteMemory(u32 addr, u32 value);
void CPUWriteHalfWord(u32 addr, u16 value);
void CPUWriteByte (u32 addr, u8  value);

u32 CPUReadMemory(u32 addr);
u16 CPUReadHalfWordSigned(u32 addr);
u8  CPUReadByte (u32 addr);


#define DEV_VERSION


bool disableMessage = false;


//ichfly i use VisualBoyAdvance instead of normal funktions because i know them




// Taken from libnds, thx !
//---------------------------------------------------------------------------------
static unsigned long ARMShift(unsigned long value,unsigned char shift) {
//---------------------------------------------------------------------------------
	// no shift at all
	if (shift == 0x0B) return value ;
	int index ;
	if (shift & 0x01) {
		// shift index is a register
		index = exceptionRegisters[(shift >> 4) & 0x0F];
	} else {
		// constant shift index
		index = ((shift >> 3) & 0x1F) ;
	} ;
	int i ;
	bool isN ;
	switch (shift & 0x06) {
		case 0x00:
			// logical left
			return (value << index) ;
		case 0x02:
			// logical right
			return (value >> index) ;
		case 0x04:
			// arithmetical right
			isN = (value & 0x80000000) ;
			value = value >> index ;
			if (isN) {
				for (i=31;i>31-index;i--) {
					value = value | (1 << i) ;
				} ;
			} ;
			return value ;
		case 0x06:
			// rotate right
			index = index & 0x1F;
			value = (value >> index) | (value << (32-index));
			return value;
	};
	return value;
}

void emuInstrARM(u32 op, s32 *R)
{
	int i;
	u32 Rn, Rd;
	u32 addr;
	u32 offset;
	
	/**
		À émuler:
	-o LDR/STR 
	-> LDRB/STRB
	-- LDRH/STRH
	-- LDRBS
	-- LDRHS
	-- LDM/STM
	-- SWP
	-- SWPB
	**/
	
	if((op & 0x0C000000) == 0x04000000) 
	{
		/* ldr/str */
		Rn = (op >> 16) & 0x7;
		Rd = (op >> 12) & 0x7;
		
		if(op & (1 << 25)) offset = ARMShift(R[op & 0xF], (op >> 4) & 0xFF); /* register (scaled) */ 
		else offset = op & 0xFFF; /* immediate */
		
		if(!(op & (1 << 23))) offset = -offset;	/* U bit */
		
		if(op & (1 << 24))
		{
			addr = R[Rn] + offset;	/* indexing */
			if(op & (1 << 21)) R[Rn] = addr; /* pre-indexing */
		}
		else
		{
			addr = R[Rn];
			R[Rn] = R[Rn] + offset; /* post-indexing */
		}
		
		if(op & (1 << 20)) R[Rd] = CPUReadMemory(addr);
		else CPUWriteMemory(addr, R[Rd]);
	}
	else if ((op & 0xFFF000F0) == 0xE1200070) //ichfly todo do it in one step
	{
	
	int comment = (op & 0x000FFF00) >> 0x8; // first is ignored 
	
	switch(comment) {
	  case 0x00:
		BIOS_SoftReset();
		break;
	  case 0x01:
		BIOS_RegisterRamReset();
		break;
	  case 0x02:         //ichfly todo 2 - 7
	#ifdef DEV_VERSION
		  Log("Halt(not yet)\n");      
	#endif    
		//holdState = true;
		//holdType = -1;
		//cpuNextEvent = cpuTotalTicks;
		break;
	  case 0x03:
	#ifdef DEV_VERSION
		  Log("Stop:\n");      
	#endif    
		//holdState = true;
		//holdType = -1;
		//stopState = true;
		//cpuNextEvent = cpuTotalTicks;
		swiIntrWait(1,IE);
		break;
	  case 0x04:
	#ifdef DEV_VERSION
		  Log("IntrWait: 0x%08x,0x%08x\n",
			  R[0],
			  R[1]);      
	#endif
	
		swiIntrWait(R[0],R[1]);
		//CPUSoftwareInterrupt();
		break;    
	  case 0x05:
	#ifdef DEV_VERSION
		  Log("VBlankIntrWait:\n");
	#endif
		//CPUSoftwareInterrupt();
		swiWaitForVBlank();
		Log("exit:\n");
		break;
	  case 0x06:
		//CPUSoftwareInterrupt();
		Log("swi 6 (not yet):\n");
		break;
	  case 0x07:
		Log("swi 7 (not yet):\n");
		//CPUSoftwareInterrupt();
		break;
	  case 0x08:
		BIOS_Sqrt();
		break;
	  case 0x09:
		BIOS_ArcTan();
		break;
	  case 0x0A:
		BIOS_ArcTan2();
		break;
	  case 0x0B:
		BIOS_CpuSet();
		break;
	  case 0x0C:
		BIOS_CpuFastSet();
		break;
	  case 0x0D:
		BIOS_GetBiosChecksum();
		break;
	  case 0x0E:
		BIOS_BgAffineSet();
		break;
	  case 0x0F:
		BIOS_ObjAffineSet();
		break;
	  case 0x10:
		BIOS_BitUnPack();
		break;
	  case 0x11:
		BIOS_LZ77UnCompWram();
		break;
	  case 0x12:
		BIOS_LZ77UnCompVram();
		break;
	  case 0x13:
		BIOS_HuffUnComp();
		break;
	  case 0x14:
		BIOS_RLUnCompWram();
		break;
	  case 0x15:
		BIOS_RLUnCompVram();
		break;
	  case 0x16:
		BIOS_Diff8bitUnFilterWram();
		break;
	  case 0x17:
		BIOS_Diff8bitUnFilterVram();
		break;
	  case 0x18:

		BIOS_Diff16bitUnFilter();
		break;
	  case 0x19:
	#ifdef DEV_VERSION
		  Log("SoundBiasSet: 0x%08x \n",
			  R[0]);      
	#endif    
		//if(reg[0].I) //ichfly sound todo
		  //systemSoundPause(); //ichfly sound todo
		//else //ichfly sound todo
		  //systemSoundResume(); //ichfly sound todo
		break;
	  case 0x1F:
		BIOS_MidiKey2Freq();
		break;
	  case 0x2A:
		BIOS_SndDriverJmpTableCopy();
		// let it go, because we don't really emulate this function
	  default:
	#ifdef DEV_VERSION
		if(systemVerbose & VERBOSE_SWI) {
		  Log("SWI: %08x (0x%08x,0x%08x,0x%08x)\n", comment,
			  R[0],
			  R[1],
			  R[2]);
		}
	#endif
		
		if(!disableMessage) {
		  systemMessage(MSG_UNSUPPORTED_BIOS_FUNCTION,
						N_("Unsupported BIOS function %02x. A BIOS file is needed in order to get correct behaviour."),
						comment);
		  disableMessage = true;
		}
		break;
	  }
	}
	else
	{
		Log("Unh. ARM: %08X\n", op);
		while(1);
	}
}

void emuInstrTHUMB(u16 op, s32 *R)
{
	int i;
	u32 Rd;
	u32 addr;
	
	if((op & 0xF000) == 0x6000)
	{
		// ldr/str R, [R, #]
		addr = R[(op >> 3) & 0x7] + ((op >> 6) & 0x1F) * 4;
		Rd = (op) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = CPUReadMemory(addr);
		else CPUWriteMemory(addr, R[Rd]);
	}
	else if ((op & 0xF800) == 0x4800)
	{
		// ldr/str R, [pc, #]
		addr = (R[15] & ~1) + (op & 0xff) * 4;
		Rd = (op >> 8) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = CPUReadMemory(addr);
		else CPUWriteMemory(addr, R[Rd]);
		
	}
	else if ((op & 0xF000) == 0x9000)
	{
		// ldr/str R, [sp, #]
		addr = R[13] + (op & 0xff) * 4;
		Rd = (op >> 8) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = CPUReadMemory(addr);
		else CPUWriteMemory(addr, R[Rd]);
		
	} 
	else if ((op & 0xF200) == 0x5000)
	{
		// ldr/str R, [R, R]
		Rd = (op) & 0x7;
		addr = R[(op >> 3) & 0x7] + R[(op >> 6) & 0x7];
		if((op & 0x400) == 0x400) R[Rd] = CPUReadMemory(addr);
		else CPUWriteMemory(addr, R[Rd]);
	}
	else if ((op & 0xF200) == 0x5200)
	{
		// ldrsh R, [R, R]
		Rd = (op) & 0x7;
		addr = R[(op >> 3) & 0x7] + R[(op >> 6) & 0x7];
		R[Rd] = (s32)CPUReadHalfWordSigned(addr);
	}
	else if ((op & 0xF000) == 0x8000)
	{
		// ldrh/strh R, [R, #]
		addr = R[(op >> 3) & 0x7] + ((op >> 6) & 0x1F) * 2;
		Rd = (op) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = (s32)CPUReadHalfWordSigned(addr);
		else CPUWriteHalfWord(addr, (s16)(R[Rd] & 0xFFFF));
	}
	else if ((op & 0xF600) == 0x5200)
	{
		// ldrh/strh R, [R, R]
		Rd = (op) & 0x7;
		addr = R[(op >> 3) & 0x7] + R[(op >> 6) & 0x7];
		if((op & 0x400) == 0x400) R[Rd] = (s32)CPUReadByte(addr);
		else CPUWriteByte(addr, (s8)(R[Rd] & 0xFF));
	}
	else if ((op & 0xF000) == 0x7000)
	{
		// ldrb/strb R, [R, #]
		addr = R[(op >> 3) & 0x7] + ((op >> 6) & 0x1F);
		Rd = (op) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = (s32)CPUReadByte(addr);
		else CPUWriteByte(addr, (s8)(R[Rd] & 0xFF));
	}
	else if ((op & 0xF600) == 0x5400)
	{
		// ldrb/strb R, [R, R]
		Rd = (op) & 0x7;
		addr = R[(op >> 3) & 0x7] + R[(op >> 6) & 0x7];
		if((op & 0x400) == 0x400) R[Rd] = (s32)CPUReadHalfWordSigned(addr);
		else CPUWriteHalfWord(addr, (s16)(R[Rd] & 0xFFFF));
	}
	else if ((op & 0xF700) == 0xB500) 
	{
		// push/pop {R,R,R,...}
		if((op & 0x400) == 0x400)	/* pop */
		{
			addr = R[13];
			for(i = 0; i <= 7; i++)
			{
				if(op & 1)
				{
					R[i] = CPUReadMemory(addr);
					addr += 4;
				}
				op = op >> 1;
			}
			if(op & 1)	/* lr is in the list */
			{
				R[14] = CPUReadMemory(addr);
				addr += 4;
			}
			R[13] = addr - 4;
		}
		else
		{
			addr = R[13];
			if(op & 0x100)	/* lr is in the list */
			{
				CPUWriteMemory(addr, R[14]);
				addr -= 4;
			}
			for(i = 7; i >= 0; i++)	/* push */
			{
				op = op << 1;
				if(op & 0x100)
				{
					CPUWriteMemory(addr, R[i]);
					addr -= 4;
				}
			}
			R[13] = addr + 4;
		}
	} 
	else if ((op & 0xF000) == 0xC000) 
	{
		// ldmia/stmia R!, {R, R, R...}
		Rd = (op >> 8) & 0x7;
		addr = R[Rd];
		if((op & 0x400) == 0x400)	/* ldmia */
		{
			for(i = 0; i <= 7; i++)
			{
				if(op & 1)
				{
					R[i] = CPUReadMemory(addr);
					addr += 4;
				}
				op = op >> 1;
			}
		}
		else
		{
			for(i = 0; i <= 7; i++)	/* stmia */
			{
				if(op & 1)
				{
					CPUWriteMemory(addr, R[i]);
					addr += 4;
				}
				op = op >> 1;
			}
		}
		R[Rd] = addr - 4;
	}
	else
	{
		Log("Unh. THUMB: %04X\n", op);
		while(1);
	}
}


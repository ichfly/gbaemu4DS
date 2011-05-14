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
	else
	{
	int opcode = op;
  switch(((opcode>>16)&0xFF0) | ((opcode>>4)&0x0F)) {
  case 0x00b:
  case 0x02b:
    {
      // STRH Rd, [Rn], -Rm
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = R[opcode & 0x0F];
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
      address -= offset;
      R[base] = address;
    }
    break;
  case 0x04b:
  case 0x06b:
    {
      // STRH Rd, [Rn], #-offset
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = (opcode & 0x0F) | ((opcode >> 4) & 0xF0);
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
      address -= offset;
      R[base] = address;
    }
    break;
  case 0x08b:
  case 0x0ab:
    {
      // STRH Rd, [Rn], Rm
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = R[opcode & 0x0F];
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
      address += offset;
      R[base] = address;
    }
    break;
  case 0x0cb:
  case 0x0eb:
    {
      // STRH Rd, [Rn], #offset
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = (opcode & 0x0F) | ((opcode >> 4) & 0xF0);
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0);
      address += offset;
      R[base] = address;
    }
    break;
  case 0x10b:
    {
      // STRH Rd, [Rn, -Rm]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - R[opcode & 0x0F];
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
    }
    break;
  case 0x12b:
    {
      // STRH Rd, [Rn, -Rm]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - R[opcode & 0x0F];
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
      R[base] = address;
    }
    break;
  case 0x14b:
    {
      // STRH Rd, [Rn, -#offset]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - ((opcode & 0x0F)|((opcode>>4)&0xF0));
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
    }
    break;
  case 0x16b:
    {
      // STRH Rd, [Rn, -#offset]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - ((opcode & 0x0F)|((opcode>>4)&0xF0));
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
      R[base] = address;
    }
    break;
  case 0x18b:
    {
      // STRH Rd, [Rn, Rm]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + R[opcode & 0x0F];
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
    }
    break;
  case 0x1ab:
    {
      // STRH Rd, [Rn, Rm]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + R[opcode & 0x0F];
      CPUWriteHalfWord(address,((reg_pair*)R)[dest].W.W0); 
      R[base] = address;
    }
    break;
  case 0x1cb:
    {
      // STRH Rd, [Rn, #offset]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + ((opcode & 0x0F)|((opcode>>4)&0xF0));
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
    }
    break;
  case 0x1eb:
    {
      // STRH Rd, [Rn, #offset]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + ((opcode & 0x0F)|((opcode>>4)&0xF0));
      CPUWriteHalfWord(address, ((reg_pair*)R)[dest].W.W0); 
      R[base] = address;
    }
    break;
  case 0x01b:
  case 0x03b:
    {
      // LDRH Rd, [Rn], -Rm
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = R[opcode & 0x0F];
      R[dest] = CPUReadHalfWord(address);
      if(dest != base) {
        address -= offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x05b:
  case 0x07b:
    {
      // LDRH Rd, [Rn], #-offset
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = (opcode & 0x0F) | ((opcode >> 4) & 0xF0);
      R[dest] = CPUReadHalfWord(address);
      if(dest != base) {
        address -= offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x09b:
  case 0x0bb:
    {
      // LDRH Rd, [Rn], Rm
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = R[opcode & 0x0F];
      R[dest] = CPUReadHalfWord(address);
      if(dest != base) {
        address += offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x0db:
  case 0x0fb:
    {
      // LDRH Rd, [Rn], #offset
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = (opcode & 0x0F) | ((opcode >> 4) & 0xF0);
      R[dest] = CPUReadHalfWord(address);
      if(dest != base) {
        address += offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x11b:
    {
      // LDRH Rd, [Rn, -Rm]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - R[opcode & 0x0F];
      R[dest] = CPUReadHalfWord(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x13b:
    {
      // LDRH Rd, [Rn, -Rm]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - R[opcode & 0x0F];
      R[dest] = CPUReadHalfWord(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x15b:
    {
      // LDRH Rd, [Rn, -#offset]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = CPUReadHalfWord(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x17b:
    {
      // LDRH Rd, [Rn, -#offset]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = CPUReadHalfWord(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x19b:
    {
      // LDRH Rd, [Rn, Rm]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + R[opcode & 0x0F];
      R[dest] = CPUReadHalfWord(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1bb:
    {
      // LDRH Rd, [Rn, Rm]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + R[opcode & 0x0F];
      R[dest] = CPUReadHalfWord(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1db:
    {
      // LDRH Rd, [Rn, #offset]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = CPUReadHalfWord(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1fb:
    {
      // LDRH Rd, [Rn, #offset]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = CPUReadHalfWord(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x01d:
  case 0x03d:
    {
      // LDRSB Rd, [Rn], -Rm
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = R[opcode & 0x0F];
      R[dest] = (s8)CPUReadByte(address);
      if(dest != base) {
        address -= offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x05d:
  case 0x07d:
    {
      // LDRSB Rd, [Rn], #-offset
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = (opcode & 0x0F) | ((opcode >> 4) & 0xF0);
      R[dest] = (s8)CPUReadByte(address);
      if(dest != base) {
        address -= offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x09d:
  case 0x0bd:
    {
      // LDRSB Rd, [Rn], Rm
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = R[opcode & 0x0F];
      R[dest] = (s8)CPUReadByte(address);
      if(dest != base) {
        address += offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x0dd:
  case 0x0fd:
    {
      // LDRSB Rd, [Rn], #offset
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = (opcode & 0x0F) | ((opcode >> 4) & 0xF0);
      R[dest] = (s8)CPUReadByte(address);
      if(dest != base) {
        address += offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x11d:
    {
      // LDRSB Rd, [Rn, -Rm]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - R[opcode & 0x0F];
      R[dest] = (s8)CPUReadByte(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x13d:
    {
      // LDRSB Rd, [Rn, -Rm]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - R[opcode & 0x0F];
      R[dest] = (s8)CPUReadByte(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x15d:
    {
      // LDRSB Rd, [Rn, -#offset]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = (s8)CPUReadByte(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x17d:
    {
      // LDRSB Rd, [Rn, -#offset]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = (s8)CPUReadByte(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x19d:
    {
      // LDRSB Rd, [Rn, Rm]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + R[opcode & 0x0F];
      R[dest] = (s8)CPUReadByte(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1bd:
    {
      // LDRSB Rd, [Rn, Rm]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + R[opcode & 0x0F];
      R[dest] = (s8)CPUReadByte(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1dd:
    {
      // LDRSB Rd, [Rn, #offset]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = (s8)CPUReadByte(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1fd:
    {
      // LDRSB Rd, [Rn, #offset]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = (s8)CPUReadByte(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x01f:
  case 0x03f:
    {
      // LDRSH Rd, [Rn], -Rm
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = R[opcode & 0x0F];
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest != base) {
        address -= offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x05f:
  case 0x07f:
    {
      // LDRSH Rd, [Rn], #-offset
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = (opcode & 0x0F) | ((opcode >> 4) & 0xF0);
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest != base) {
        address -= offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x09f:
  case 0x0bf:
    {
      // LDRSH Rd, [Rn], Rm
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = R[opcode & 0x0F];
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest != base) {
        address += offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x0df:
  case 0x0ff:
    {
      // LDRSH Rd, [Rn], #offset
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base];
      int offset = (opcode & 0x0F) | ((opcode >> 4) & 0xF0);
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest != base) {
        address += offset;
        R[base] = address;
      }
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x11f:
    {
      // LDRSH Rd, [Rn, -Rm]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - R[opcode & 0x0F];
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x13f:
    {
      // LDRSH Rd, [Rn, -Rm]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - R[opcode & 0x0F];
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x15f:
    {
      // LDRSH Rd, [Rn, -#offset]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x17f:
    {
      // LDRSH Rd, [Rn, -#offset]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] - ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x19f:
    {
      // LDRSH Rd, [Rn, Rm]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + R[opcode & 0x0F];
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1bf:
    {
      // LDRSH Rd, [Rn, Rm]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + R[opcode & 0x0F];
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1df:
    {
      // LDRSH Rd, [Rn, #offset]
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
  case 0x1ff:
    {
      // LDRSH Rd, [Rn, #offset]!
      int base = (opcode >> 16) & 0x0F;
      int dest = (opcode >> 12) & 0x0F;
      u32 address = R[base] + ((opcode & 0x0F)|((opcode>>4)&0xF0));
      R[dest] = (s16)CPUReadHalfWordSigned(address);
      if(dest != base)
        R[base] = address;
      if(dest == 15) {
        R[15] &= 0xFFFFFFFC;
        armNextPC = R[15];
        R[15] += 4;
        //ARM_PREFETCH;
      }
    }
    break;
	default:
	{
		Log("Unh. ARM: %08X\n", op);
		while(1);
	}
	}
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


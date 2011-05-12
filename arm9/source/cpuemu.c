#include "main.h"

void Write32(u32 addr, u32 value);
void Write16(u32 addr, u16 value);
void Write8 (u32 addr, u8  value);

u32 Read32(u32 addr);
u16 Read16(u32 addr);
u8  Read8 (u32 addr);

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
		
		if(op & (1 << 20)) R[Rd] = Read32(addr);
		else Write32(addr, R[Rd]);
	}
	else
	{
		Log("Unh. ARM: %08X\n", op);
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
		if((op & 0x400) == 0x400) R[Rd] = Read32(addr);
		else Write32(addr, R[Rd]);
	}
	else if ((op & 0xF800) == 0x4800)
	{
		// ldr/str R, [pc, #]
		addr = (R[15] & ~1) + (op & 0xff) * 4;
		Rd = (op >> 8) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = Read32(addr);
		else Write32(addr, R[Rd]);
		
	}
	else if ((op & 0xF000) == 0x9000)
	{
		// ldr/str R, [sp, #]
		addr = R[13] + (op & 0xff) * 4;
		Rd = (op >> 8) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = Read32(addr);
		else Write32(addr, R[Rd]);
		
	} 
	else if ((op & 0xF200) == 0x5000)
	{
		// ldr/str R, [R, R]
		Rd = (op) & 0x7;
		addr = R[(op >> 3) & 0x7] + R[(op >> 6) & 0x7];
		if((op & 0x400) == 0x400) R[Rd] = Read32(addr);
		else Write32(addr, R[Rd]);
	}
	else if ((op & 0xF200) == 0x5200)
	{
		// ldrsh R, [R, R]
		Rd = (op) & 0x7;
		addr = R[(op >> 3) & 0x7] + R[(op >> 6) & 0x7];
		R[Rd] = (s32)Read16(addr);
	}
	else if ((op & 0xF000) == 0x8000)
	{
		// ldrh/strh R, [R, #]
		addr = R[(op >> 3) & 0x7] + ((op >> 6) & 0x1F) * 2;
		Rd = (op) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = (s32)Read16(addr);
		else Write16(addr, (s16)(R[Rd] & 0xFFFF));
	}
	else if ((op & 0xF600) == 0x5200)
	{
		// ldrh/strh R, [R, R]
		Rd = (op) & 0x7;
		addr = R[(op >> 3) & 0x7] + R[(op >> 6) & 0x7];
		if((op & 0x400) == 0x400) R[Rd] = (s32)Read8(addr);
		else Write16(addr, (s8)(R[Rd] & 0xFF));
	}
	else if ((op & 0xF000) == 0x7000)
	{
		// ldrb/strb R, [R, #]
		addr = R[(op >> 3) & 0x7] + ((op >> 6) & 0x1F);
		Rd = (op) & 0x7;
		if((op & 0x400) == 0x400) R[Rd] = (s32)Read8(addr);
		else Write16(addr, (s8)(R[Rd] & 0xFF));
	}
	else if ((op & 0xF600) == 0x5400)
	{
		// ldrb/strb R, [R, R]
		Rd = (op) & 0x7;
		addr = R[(op >> 3) & 0x7] + R[(op >> 6) & 0x7];
		if((op & 0x400) == 0x400) R[Rd] = (s32)Read16(addr);
		else Write16(addr, (s16)(R[Rd] & 0xFFFF));
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
					R[i] = Read32(addr);
					addr += 4;
				}
				op = op >> 1;
			}
			if(op & 1)	/* lr is in the list */
			{
				R[14] = Read32(addr);
				addr += 4;
			}
			R[13] = addr - 4;
		}
		else
		{
			addr = R[13];
			if(op & 0x100)	/* lr is in the list */
			{
				Write32(addr, R[14]);
				addr -= 4;
			}
			for(i = 7; i >= 0; i++)	/* push */
			{
				op = op << 1;
				if(op & 0x100)
				{
					Write32(addr, R[i]);
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
					R[i] = Read32(addr);
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
					Write32(addr, R[i]);
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
	}
}


#include <nds.h>
#include <stdio.h>

#include <filesystem.h>
#include "unzip.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include "ram.h"
#include <dirent.h>

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

void VblankHandler();

void BIOScall(int op,  s32 *R);


#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "unzip.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include "ram.h"
#include <dirent.h>

#include "cpumg.h"
#include "GBAinline.h"


#include "main.h"






#define releas





	FILE * pFile;


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


extern bool disableMessage;



















#include "main.h"
#include "cpumg.h"

#define PU_PAGE_4K		(0x0B << 1)
#define PU_PAGE_8K		(0x0C << 1)
#define PU_PAGE_16K		(0x0D << 1)
#define PU_PAGE_32K		(0x0E << 1)
#define PU_PAGE_64K		(0x0F << 1)
#define PU_PAGE_128K		(0x10 << 1)
#define PU_PAGE_256K		(0x11 << 1)
#define PU_PAGE_512K		(0x12 << 1)
#define PU_PAGE_1M		(0x13 << 1)
#define PU_PAGE_2M		(0x14 << 1)
#define PU_PAGE_4M		(0x15 << 1)
#define PU_PAGE_8M		(0x16 << 1)
#define PU_PAGE_16M		(0x17 << 1)
#define PU_PAGE_32M		(0x18 << 1)
#define PU_PAGE_64M		(0x19 << 1)
#define PU_PAGE_128M		(0x1A << 1)
#define PU_PAGE_256M		(0x1B << 1)
#define PU_PAGE_512M		(0x1C << 1)
#define PU_PAGE_1G		(0x1D << 1)
#define PU_PAGE_2G		(0x1E << 1)
#define PU_PAGE_4G		(0x1F << 1)

// extern void puSetMemPerm(u32 perm);
extern "C" void pu_Enable();
// extern void puSetGbaIWRAM();
extern "C" void pu_SetRegion(u32 region, u32 value);

extern "C" void pu_SetDataPermissions(u32 v);
extern "C" void pu_SetCodePermissions(u32 v);
extern "C" void  pu_SetDataCachability(u32 v);
extern "C" void  pu_SetCodeCachability(u32 v);
extern "C" void pu_GetWriteBufferability(u32 v);

u16 gbaIME = 0;
u16 gbaDISPCNT = 0;
u16 gbaBGxCNT[4] = {0, 0, 0, 0};

extern "C" void exMain(); 

extern void __attribute__((section(".itcm"))) (*exHandler)();
extern s32  __attribute__((section(".itcm"))) exRegs[];

#define BIOSDBG_CP15 *((volatile u32*)0x027FFD8C)
#define BIOSDBG_SPSR *((volatile u32*)0x027FFD90)
#define BIOSDBG_R12  *((volatile u32*)0x027FFD94)
#define BIOSDBG_PC   *((volatile u32*)0x027FFD98)






/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

#define do_div(n,base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })

static char * number(char * str, long num, int base, int size, int precision
	,int type)
{
	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0)
		tmp[i++] = digits[do_div(num,base)];
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type & SPECIAL) {
		if (base==8)
			*str++ = '0';
		else if (base==16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;
	while (i < precision--)
		*str++ = '0';
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';
	return str;
}



int kvsprintf(char *buf, const char *fmt, va_list args)
{
	int len;
	unsigned long num;
	int i, base;
	char * str;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;


		case 'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int * ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		case '%':
			*str++ = '%';
			continue;

		/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = '\0';
	return str-buf;
}











int durchlauf = 0;



void Logsd(const char *defaultMsg,...)
{
char buffer[256];
	  va_list valist;
  va_start(valist, defaultMsg);
  //iprintf("%s",defaultMsg);
  //while(1);
  kvsprintf(buffer, defaultMsg, valist); //workaround

	iprintf(buffer);
    //fputs(buffer, pFile);

  
  va_end(valist);
  
}

void exInit(void (*customHdl)())
{
	EXCEPTION_VECTOR = exMain;
	exHandler = customHdl;
}
/*
void Write32(u32 address, u32 value)
{
	Log("Write32: *%08X = %08X\n", address, value);
	if((address & 0x0F000000) == 0x04000000)	/* IO *//*
	{
		switch(address & 0xFFF)
		{
			case 0x208:	/* REG_IME *//*
				gbaIME = value & 0x1; 
				break;
			default:
				Log("Unh. IO Write32 at %08X\n", address);
				break;
		}
	}			
}

#define					BG0_CR		(*(vu16*)0x4000008)
#define DISPLAY_CR (*(vu32*)0x04000000)

void Write16(u32 address, u16 value)
{
	Log("Write16: *%08X = %04X\n", address, value);
	if((address & 0x0F000000) == 0x04000000)	/* IO *//*
	{
		switch(address & 0xFFF)
		{
			case 0x000: {	/* REG_DISPCNT *//*
				u32 dsValue;
				gbaDISPCNT = value;
				dsValue  = value & 0xFF87;
				dsValue |= (value & (1 << 5)) ? (1 << 23) : 0;	/* oam hblank access *//*
				dsValue |= (value & (1 << 6)) ? (1 << 4) : 0;	/* obj mapping 1d/2d *//*
				dsValue |= (value & (1 << 7)) ? 0 : (1 << 16);	/* forced blank => no display mode *//*
				/* TODO: gérer mode 4 *//*
				REG_DISPCNT = dsValue;
			} break;
			case 0x004: /* REG_DISPSTAT *//*
				/* TODO *//*
				break;
			case 0x008: /* REG_BG0CNT *//*
			case 0x00A: /* REG_BG1CNT *//*
			case 0x00C: /* REG_BG2CNT *//*
			case 0x00E: { /* REG_BG3CNT *//*
				u16 dsValue;
				int bg = (address - 0x4000008) >> 1;
				gbaBGxCNT[bg] = value;
				dsValue = value;
				*(&REG_BG0CNT + bg) = dsValue;
			} break;
			case 0x208: /* REG_IME *//*
				gbaIME = value & 0x1; 
				break;
			default:
				Log("Unh. IO Write16 at %08X\n", address);
				break;
		}
	}
}
void Write8 (u32 address, u8  value)
{
	Log("Write8 : *%08X = %02X\n", address, value);
	if((address & 0x0F000000) == 0x04000000)	/* IO *//*
	{
		switch(address & 0xFFF)
		{
			default:
				Log("Unh. IO Write8 at %08X\n", address);
				break;
		}
	}
}

u32 Read32(u32 address)
{
	u32 value = 0;
	Log("Read32: *%08X\n", address);
	if((address & 0x0F000000) == 0x04000000)	/* IO *//*
	{
		switch(address & 0xFFF)
		{
			case 0x208:
				value = gbaIME; 
				break;
			default:
				Log("Unh. IO Read32 at %08X\n", address);
				break;
		}
	}
	return value;
}

u16 Read16(u32 address)
{
	u16 value = 0;
	Log("Read16: *%08X\n", address);
	if((address & 0x0F000000) == 0x04000000)	/* IO *//*
	{
		switch(address & 0xFFF)
		{
			case 0x000: /* REG_DISPCNT *//*
				value = gbaDISPCNT; break;
			case 0x004: /* REG_DISPSTAT *//*
				/* TODO *//*
				break;
			case 0x006: /* REG_VCOUNT *//*
				value = REG_VCOUNT;
				if(value > 227) value = 227;	/* limite à la taille de la gba *//*
				break;
			case 0x008: /* REG_BG0CNT *//*
			case 0x00A: /* REG_BG1CNT *//*
			case 0x00C: /* REG_BG2CNT *//*
			case 0x00E: { /* REG_BG3CNT *//*
				int bg = (address - 0x4000008) >> 1;
				value = gbaBGxCNT[bg];
			} break;
			case 0x208: /* REG_IME *//*
				value = gbaIME;  break;
			default:
				Log("Unh. IO Read16 at %08X\n", address);
				break;
		}
	}
	return value;
}
u8  Read8 (u32 address)
{
	u8 value = 0;
	Log("Read8 : *%08X\n", address);
	if((address & 0x0F000000) == 0x04000000)	/* IO *//*
	{
		switch(address & 0xFFF)
		{
			default:
				Log("Unh. IO Read8 at %08X\n", address);
				break;
		}
	}
	return value;
}
*/
void emuInstrARM(u32 instr, s32 *regs);
void emuInstrTHUMB(u16 instr, s32 *regs);

#define B8(h,g,f,e,d,c,b,a) ((a)|((b)<<1)|((c)<<2)|((d)<<3)|((e)<<4)|((f)<<5)|((g)<<6)|((h)<<7))

void puGba()
{
	/* NDS PU REGIONS: 
	0: io + vram
	1: bios
	2: alternate vector base
	3: DTCM
	4: ITCM
	5: new 0x300000 (old gba slot)
	6: non cacheable main ram
	7: cacheable main ram
	*/
	
	
	//REG_IME = IME_DISABLE;
	
	//DC_FlushRange((void*)0x02000000, 4 * 1024 * 1024); //slowdown and is not realy needed
	//pu_SetDataCachability(   B8(0,0,0,0,0,0,1,0)); //ichfly todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//pu_SetCodeCachability(   B8(0,0,0,0,0,0,1,0));
	//pu_GetWriteBufferability(B8(0,0,0,0,0,0,0,0));
	
	//pu_SetRegion(0, 0x00000000 | PU_PAGE_4G | 1);	/* fond */ //ichfly wtf
	//pu_SetRegion(1, 0x04000000 | PU_PAGE_16M | 1);	/* io */
	
	//pu_SetDataPermissions(0x36333303);
	//pu_SetCodePermissions(0x36636303);
	
	pu_SetCodePermissions(0x33333360);
	
	pu_SetDataPermissions(0x33333360);

	//REG_IME = IME_ENABLE;	//lol don't enable this
	
	
}

void puNds()
{
	/* NDS PU REGIONS: 
	0: io + vram
	1: bios
	2: alternate vector base
	3: DTCM
	4: ITCM
	5: new 0x300000 (old gba slot)
	6: non cacheable main ram
	7: cacheable main ram
	*/
	
	
	pu_SetDataPermissions(0x33333363);
	pu_SetCodePermissions(0x33333363);
	

	
	//pu_SetRegion(6, 0x02000000 | PU_PAGE_16M  | 1);	/* main ram */ //why
	//pu_SetRegion(7, 0x02000000 | PU_PAGE_4M  | 1);	/* main rams */
	//pu_SetRegion(0, 0x04000000 | PU_PAGE_64M | 1);	/* io + vram */
	
	/*pu_SetDataCachability(   B8(1,0,0,0,0,0,1,0)); //ichfly todo
	pu_SetCodeCachability(   B8(1,0,0,0,0,0,1,0));
	pu_GetWriteBufferability(B8(1,0,0,0,0,0,0,0));*/
	
	//REG_IME = IME_ENABLE;	//ichfy for test
}

static inline void debugDump()
{
// 	Log("dbgDump\n");
// 	return;
	
	int i;
	for(i = 0; i <= 15; i++) {
		Log("R%d=%X ", i, exRegs[i]);
	} 
	Log("\n");
	for(i = 0; i < 4; i++) {
		Log("+%02X: %08X %08X %08X\n", i*3*4, ((u32*)exRegs[13])[i*3], ((u32*)exRegs[13])[i*3+1], ((u32*)exRegs[13])[i*3+2]);
	}
}

int durchgang = 0;

void gbaExceptionHdl()
{


	//todo sem the 0x3000000 is copyed to the wrong location 

	int i;
	u32 instr;
	u32 sysMode;
	u32 cpuMode;
	u32 opSize;
	
	ndsMode();
	sysMode = cpuGetCPSR() & 0x1F;
	cpuMode = BIOSDBG_SPSR & 0x20;
	
	if(cpuMode) opSize = 2;
	else opSize = 4;
	

	
	//exRegs[15] -= 4; //ichfly patch not working on emulators
	

	//exRegs[15] += 4;
		
	
	//while(1);
	
	  durchlauf++;
	//if(durchlauf == 0x8000)while(1);
	
	Log("%08X %08X\n", exRegs[15] , durchlauf);
	//Log("%08X\n", exRegs[0]);
	if(exRegs[15] < 0x02000000)while(1);
	if(exRegs[15] > 0x04000000 && !(exRegs[15] & 0x08000000))while(1);

	//debugDump();


	
	if(exRegs[15] & 0x08000000)
	{
		//if(exRegs[15] == 0x08000290)while(1);
		//Logsd("%08X\n", exRegs[15]);
		//debugDump();
		BIOSDBG_SPSR |= 0x20;
		//exRegs[15] -= 8; //for my emu
		//exRegs[15] -= 4; //for nothing
		exRegs[15] = (exRegs[15] & 0x07FFFFFF) + (s32)rom;
		
		
		
	
	}
	else
	{
		if(sysMode == 0x17)
		{
	 //		Logsd("-------- DA :\n");

			
			//exRegs[15] += 4; //for emu
			
			//debugDump();
			
			//durchgang++;
			
			//iprintf("%08X %X (%08X)\n", exRegs[15],cpuMode,*(u32*)(exRegs[15] - 8));
			
			
				//debugDump();
			
			if(cpuMode)
			{
				instr = *(u16*)(exRegs[15] - 8);
				exRegs[15] -= 2; //won't work
				//BIOSDBG_PC -= 2; 
			}
			else
			{
				instr = *(u32*)(exRegs[15] - 8);
				//exRegs[15] -= 2;
				//
			}
			/*if(cpuMode) instr = (u32)*(u16*)(exRegs[15] - 4);
			else instr = *(u32*)(exRegs[15] - 4);*/
			
			
			


			
			if(cpuMode)
			{
				u16 tempforwtf = *(u16*)(exRegs[15] - 2);

				//Logsd("%08X\n", instr);
				if(tempforwtf > 0xBE00 && tempforwtf < 0xBE2B)
				{
				
					
					exRegs[15] += 4;
					//debugDump();
					BIOScall(tempforwtf,  exRegs);
					
					
					
					//while(1);
					//debugDump();
				}
				else emuInstrTHUMB(instr, exRegs);
				//exRegs[15] -= 2;
				
			}
			else
			{
			
				u32 tempforwtf = *(u32*)(exRegs[15] - 4);

				//Logsd("%08X\n", instr);
				if((tempforwtf &0xFFF000F0) == 0xE1200070)
				{
					exRegs[15] += 4;
					BIOScall((tempforwtf & 0xFFF00)>>0x8, exRegs);
				}
	// 			Logsd("ARM: %08X\n", instr);
				emuInstrARM(instr, exRegs);
	// 			Logsd("NDS TRACE\n")
				//exRegs[15] -= 4;
			}
			
	// 		exRegs[15] += opSize;
	// 		while(1) { }
		}
		else if(sysMode == 0x1B)
		{
			if(cpuMode) instr = (u32)*(u16*)(exRegs[15] - 2);
			else instr = *(u32*)(exRegs[15] - 4);
			
			/* Undefined instruction (debug...) */
			if((!cpuMode && instr == 0xE7F000F0) || (cpuMode && instr == 0xDE00))
			{
				Log("Trace... [%s]\n", cpuMode ? "THUMB" : "ARM");
			}
			else if((!cpuMode && instr == 0xE7F000F1) || (cpuMode && instr == 0xDE01))
			{
				debugDump();
			}
			else
			{
				Log("Unhandled und. except. (%08X)\n", instr);
				//while(1)swiWaitForVBlank();
			}
			
			exRegs[15] += 4;
		}
	}

		//while(1);
	
	//Log("%08X\n", exRegs[1]);
	//Log("%08X\n", exRegs[0]);
	//while(1);
	//debugDump();
	
	//test mode
	
	
	/*iprintf("test\r\n%x\r\n%x\r\n%x\r\n%x\r\n",BIOSDBG_CP15, BIOSDBG_SPSR, BIOSDBG_R12, BIOSDBG_PC);
	
	BIOSDBG_PC = 0;
	
	iprintf("%x", BIOSDBG_PC);
	while(1);*/
	//test mode end
	
	//Log("%08X\n", exRegs[15]);
	
	//debugDump();
	
	gbaMode();
	
	//if(*(u16*)(exRegs[15] - 2) == 0xBE05) while(1);
			//swiDelay(0x2000000); --
	//swiDelay(0x20000);
	
	//if(exRegs[15] < 0x02000000)while(1) { ; } //i was funny hahahahaha

}


void gbaInit()
{
	//pFile = fopen("fat:/gbaemulog.log","w");
	pu_SetDataCachability(   B8(0,1,0,0,0,0,0,0)); //ichfly todo slowdown !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	pu_SetCodeCachability(   B8(0,1,0,0,0,0,0,0));
	pu_GetWriteBufferability(B8(0,0,0,0,0,0,0,0));
	
	//Logsd ("test %x\r\n",pFile);
	
	//fputs ("fopen example\r\n",pFile);
	
	// 	puSetGbaIWRAM();
	pu_SetRegion(5, 0x03000000 | PU_PAGE_32K | 1);	/* gba iwram */ //it is the GBA Cart in the original
	pu_SetRegion(6, 0x02000000 | PU_PAGE_16M | 1);    //ram
	//pu_SetRegion(7, 0x07000000 | PU_PAGE_16M | 1);
	pu_SetRegion(2, 0x05000000 | PU_PAGE_16M | 1);
	pu_SetRegion(7, 0x06000000 | PU_PAGE_32M | 1);	//todo swap
	//pu_SetRegion(2, 0x05000000 | PU_PAGE_64M | 1);
	//pu_SetRegion(3, 0x00000000 | PU_PAGE_32M | 1);
	//pu_SetRegion(4, 0x02040000 | PU_PAGE_8M | 1);
	WRAM_CR = 0;
#ifdef releas
	exInit(gbaExceptionHdl);
#endif

	iprintf("gbainit done");
	
}

void gbaMode()
{
#ifndef releas
	exInit(gbaExceptionHdl);
#endif
	puGba();
	
}

void ndsExceptionHdl()
{
	u32 mode = cpuGetCPSR() & 0x1F;
	u32 instrset = BIOSDBG_SPSR & 0x20;
	
	if(mode == 0x17) Log("NDS DATA ABORT\n");
	else if(mode == 0x1B) Log("NDS UNDEFINED INSTRUCTION\n");
	else Log("NDS STRANGE EXCEPTION !\n");
	Log("SAVED PC = %08X (%s)\n", exRegs[15], instrset ? "THUMB" : "ARM");
	if(instrset) Log("FAILED INSTR = %04X\n", *(u16*)(exRegs[15] - (mode == 0x17 ? 4 : 2)));
	else Log("FAILED INSTR = %08X\n", *(u32*)(exRegs[15] - (mode == 0x17 ? 8 : 4)));
	while(1) { ; }
}

void ndsMode()
{
	puNds();
#ifndef releas
	exInit(ndsExceptionHdl);
#endif
}
void BIOScall(int op,  s32 *R)
{
	int comment = op & 0x003F;
	
	switch(comment) {
	  case 0x00:
		BIOS_SoftReset();
		break;
	  case 0x01:
		BIOS_RegisterRamReset();
		break;
	  case 0x02:
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
		  VblankHandler();
	#endif
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		//CPUSoftwareInterrupt();
		//swiWaitForVBlank(); //is not working every time
		
		//Log("exit:\n");
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

//b
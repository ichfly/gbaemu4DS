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
	/* GBA PU REGIONS:
	0: fond inscriptible
	1: io (tout interdit)
	2: iwram
	3: gba slot
	4: dtcm (for exceptions)
	5: itcm (idem)
	6: arm9 bios (idem)
	7: shared ram (again)
	*/ //wrong !!!
	
	
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
	1: main ram (including gba ewram)
	2: gba iwram
	3: gba slot
	4: dtcm
	5: itcm
	6: arm9 bios
	7: shared ram
	*/ // wrong
	
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
	
	
	
	
	exRegs[15] -= 4; //ichfly patch not working on emulators
	
	//Log("%08X\n", exRegs[15]);
	
	if(exRegs[15] & 0x08000000)
	{
		exRegs[15] = (exRegs[15] & 0x07FFFFFF) + (s32)rom;
	}
	else
	{
		if(sysMode == 0x17)
		{
	 //		Log("-------- DA :\n");

			
			exRegs[15] += 4;
			
			//debugDump();
			
			durchgang++;
			
			
			
			
			if(cpuMode)
			{
				instr = *(u16*)(exRegs[15] - 8);
				exRegs[15] -= 2;
			}
			else
			{
				instr = *(u32*)(exRegs[15] - 8);
				//exRegs[15] -= 4;
			}
			/*if(cpuMode) instr = (u32)*(u16*)(exRegs[15] - 4);
			else instr = *(u32*)(exRegs[15] - 4);*/
			
			
			
			
			
			if(cpuMode)
			{
				u16 tempforwtf = *(u16*)(exRegs[15] - 2);

				//Log("%08X\n", instr);
				if(tempforwtf > 0xBE00 && tempforwtf < 0xBE2B)
				{
				
					
				
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

				//Log("%08X\n", instr);
				if(tempforwtf &0xFFF000F0) == 0xE1200070)
				{
					BIOScall((tempforwtf & 0xFFF00)<<0x8,  exRegs);
				}
	// 			Log("ARM: %08X\n", instr);
				emuInstrARM(instr, exRegs);
	// 			Log("NDS TRACE\n")
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
	gbaMode();
	//if(*(u16*)(exRegs[15] - 2) == 0xBE05) while(1);
			//swiDelay(0x2000000); --
	//swiDelay(0x20000);
	
	//if(exRegs[15] < 0x02000000)while(1) { ; } //i was funny hahahahaha
	
	
}


void gbaInit()
{
// 	puSetGbaIWRAM();
	pu_SetRegion(5, 0x03000000 | PU_PAGE_32K | 1);	/* gba iwram */ //it is the GBA Cart in the original
	WRAM_CR = 0;
	
	
	pu_SetDataCachability(   B8(0,0,0,0,0,0,0,0)); //ichfly todo slowdown !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	pu_SetCodeCachability(   B8(0,0,0,0,0,0,0,0));
	pu_GetWriteBufferability(B8(0,0,0,0,0,0,0,0));
}

void gbaMode()
{
	exInit(gbaExceptionHdl);
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
	exInit(ndsExceptionHdl);

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

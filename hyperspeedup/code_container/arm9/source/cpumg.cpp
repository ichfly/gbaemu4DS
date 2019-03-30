#include "cpumg.h"

#include <nds.h>
#include <stdio.h> 

#include <filesystem.h>
#include <fat.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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

#include "getopt.h"
#include "System.h"
#include "agbprint.h"

#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "System.h"
#include "main.h"
#include "armdis.h"
#include "EEprom.h"
#include "Flash.h"
#include "Sram.h"
#include "bios.h"
#include "Cheats.h"
#include "NLS.h"
#include "elf.h"
#include "gba_ipc.h"


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

char disbuffer[0x2000];

#ifdef lastdebug
	u32 lasttime[6];
	int lastdebugcurrent = 0;
	int lastdebugsize = 6;
#endif

int durchlauf = 0;
int durchgang = 0;

#define B8(h,g,f,e,d,c,b,a) ((a)|((b)<<1)|((c)<<2)|((d)<<3)|((e)<<4)|((f)<<5)|((g)<<6)|((h)<<7))


void debugDump()
{
#ifdef ichflyDebugdumpon
// 	iprintf("dbgDump\n");
// 	return;
	//readbankedextra(cpuGetSPSR());

	int i;
	for(i = 0; i <= 15; i++) {
		iprintf("R%d=%X ", (int)i, (unsigned int)exRegs[i]);
	} 
	iprintf("\n");
	iprintf("sup %X %X\n",SPtoload,SPtemp);

	/*if((exRegs[13] &0xFF000000) != 0x3000000)
	{
		REG_IME = IME_DISABLE;
		while(1);
	}*/
DC_FlushAll();
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu else we may cause an endless loop

	for(i = 0; i < 4; i++) {
		iprintf("+%02X: %08X %08X %08X\n", (int)(i*3*4), (unsigned int)((u32*)exRegs[13])[i*3], (unsigned int)((u32*)exRegs[13])[i*3+1], (unsigned int)((u32*)exRegs[13])[i*3+2]);
	}

	pu_Enable(); //back to normal code
DC_FlushAll();
	iprintf("SPSR %08x ", (unsigned int)BIOSDBG_SPSR);
	iprintf("CPSR %08x\n",(unsigned int)cpuGetCPSR());
	//iprintf("irqBank %08x",readbankedsp(0x12));
	iprintf("irqBank %08x %08x\n",(unsigned int)readbankedlr(0x12),(unsigned int)readbankedsp(0x12));
	iprintf("userBank %08x %08x\n",(unsigned int)readbankedlr(0x1F),(unsigned int)readbankedsp(0x1F));
#ifdef lastdebug
	int ipr = lastdebugcurrent - 1;
	if(ipr < 0)ipr= lastdebugsize - 1;
	while(ipr != lastdebugcurrent)
	{
		iprintf("run %08X\n",(unsigned int)lasttime[ipr]);
		ipr--;
		if(ipr < 0)ipr= lastdebugsize - 1;
	}
	iprintf("run %08X\n",(unsigned int)lasttime[lastdebugcurrent]); //last
#endif
	iprintf("IEF: %08X\n",(unsigned int)CPUReadMemoryreal(0x4000200));
	  u32 joy = ((~REG_KEYINPUT)&0x3ff);
	if((joy & KEY_B) && (joy & KEY_R) && (joy & KEY_L))
	{
					FILE* file = fopen("fat:/gbadump.bin", "wb"); // 396.288 Byte @outdate
					fwrite((u8*)(0x03000000), 1, 0x8000, file);
					fwrite(ioMem, 1, 0x400, file);
					fwrite((u8*)(0x04000000), 1, 0x400, file);//IO
					fwrite((u8*)(0x05000000), 1, 0x400, file);
					fwrite((u8*)(0x07000000), 1, 0x800, file);
					fwrite((u8*)(0x01000000), 1, 0x8000, file);
					fwrite((u8*)(0x0b000000), 1, 0x4000, file);
					fwrite((u8*)(0x06000000), 1, 0x18000, file); //can't get this with half dumps
					fwrite((u8*)(0x02000000), 1, 0x40000, file); //can't get this with half dumps
	}
#endif
}


void failcpphandler()
{
	iprintf("something failed\r\n");
	REG_IME = IME_DISABLE;
	debugDump();
	iprintf("SSP %08x SLR %08x\n",(unsigned int)savedsp,(unsigned int)savedlr);
		while(1);
}

void exInitundifinedsystem(void (*customundifinedHdl)())
{
	exHandlerundifined = customundifinedHdl;
}

void exInitswisystem(void (*customswiHdl)())
{
	exHandlerswi = customswiHdl;
}

void exInit(void (*customHdl)())
{
	//EXCEPTION_VECTOR = exMain; //no more needed
	exHandler = customHdl;
}

void undifinedresolver()
{
	//coto: by downgrading armv5 to armv4 and running always in ARMv4 mode we prevent the below undefined exception.
	
	/*
	u32 tempforwtf = *(u32*)(exRegs[15] - 4);
	if((tempforwtf &0x0F200090) == 0x00200090) //wtf why dos this tigger an exeption it is strh r1,[r0]+2! ²àà 0xB2 10 E0 E0 on gba 0xE0E010B2 so think all strh rx,[ry]+z! do that
	{
		*(u32*)(exRegs[15] - 4) = tempforwtf & ~0x200000;//ther is just a wrong bit so don't worry patch it to strh r1,[r0]+2
	}
	*/
	//else
	{
		printf("unknown OP\r\n");
		debugDump();
		REG_IME = IME_DISABLE;
		while(1);
	}
}


void gbaInit(bool useMPUFast){
	exInitswisystem(gbaswieulatedbios); //define handler
	exInitundifinedsystem(undifinedresolver); //define handler

	REG_IME = IME_DISABLE;
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu while configurating pu
	
	exInit(gbaExceptionHdl); //define handler
	WRAM_CR = 0; //swap wram in
	
	//slower but handles most games
	if(useMPUFast == false){
		pu_SetDataCachability(   0b01111110);
		pu_SetCodeCachability(   0b01111110);
		pu_GetWriteBufferability(0b01111110);
		pu_SetRegion(0, 0x00000000 | PU_PAGE_128M | 1);
		//pu_SetRegion(1, (u32)&__dtcm_start | PU_PAGE_16K | 1);
		//pu_SetRegion(1, 0);
		pu_SetRegion(2, 0x02040000 | PU_PAGE_256K | 1);
		pu_SetRegion(3, 0x02080000 | PU_PAGE_512K | 1);
		pu_SetRegion(4, 0x02100000 | PU_PAGE_1M | 1);
		pu_SetRegion(5, 0x02200000 | PU_PAGE_2M | 1);
		pu_SetRegion(6, 0x00000000 | PU_PAGE_16M | 1);
		pu_SetRegion(7, 0x04000000 | PU_PAGE_16M | 1);
	}
	//faster but only less accurate/ screen glitches 
	else{
		pu_SetDataCachability(   0b00111110);
		pu_SetCodeCachability(   0b00111110);
		pu_GetWriteBufferability(0b00111110);
		pu_SetRegion(0, 0x00000000 | PU_PAGE_128M | 1); 
		pu_SetRegion(1, (u32)(0x02000000) 						| PU_PAGE_1M  | 1); 
		pu_SetRegion(2, (u32)(0x02100000) 						| PU_PAGE_1M  | 1); 
		pu_SetRegion(3, (u32)(0x02200000)						| PU_PAGE_1M | 1);  
		pu_SetRegion(4, (u32)(0x02300000)  					| PU_PAGE_2M | 1);
		pu_SetRegion(5, (u32)(0x01FF8000) 						| PU_PAGE_32K  | 1); 
		pu_SetRegion(6, 0x00000000 | PU_PAGE_16M | 1); //vector protection
		pu_SetRegion(7, 0x04000000 | PU_PAGE_16M | 1); //IO protection
	}
	
	pu_Enable(); //PU go
	DC_FlushAll(); //try it
	IC_InvalidateAll();	
}

void gbaswieulatedbios()
{
	//iprintf("\n\rswi\n\r");
	//debugDump();

	//while(1);

	//iprintf("%08X S\n", readbankedsp(0x12));

	u16 tempforwtf = *(u16*)(exRegs[15] - 2);
	BIOScall(tempforwtf,  exRegs);

	#ifdef lastdebug
		if(readbankedsp(0x12) < 0x1000000)
			debugandhalt();
		lasttime[lastdebugcurrent] = exRegs[15] | 0x80000000;
		lastdebugcurrent++;
		if(lastdebugcurrent == lastdebugsize)
			lastdebugcurrent = 0;
	#endif

	gbaMode();
	//while(1);
}

__attribute__((section(".itcm")))
void BIOScall(int op,  u32 *R){
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
	    iprintf("Halt: IE %x\n",(unsigned int)IE);
#endif
		//holdState = true;
		//holdType = -1;
		//cpuNextEvent = cpuTotalTicks;
		
		ichflyswiHalt();
		//durchlauf = 1;
		
		//debugDump();
		
		//VblankHandler();
		
		break;
	  case 0x03:{
#ifdef DEV_VERSION
		  iprintf("Stop\n");
#endif
			//holdState = true;
			//holdType = -1;
			//stopState = true;
			//cpuNextEvent = cpuTotalTicks;
			
			//sleepmode swi 0x3 gba
			SendArm7Command((u32)ARM9_REQ_SWI_TO_ARM7, (u32)IRQ_LID);
	  }
      break;
	  case 0x04:
#ifdef DEV_VERSION
		  iprintf("IntrWait: 0x%08x,0x%08x\n",
			  (unsigned int)R[0],
			  (unsigned int)R[1]);      
#endif
		ichflyswiIntrWait(R[0],R[1]);
		//CPUSoftwareInterrupt();
		break;    
	  case 0x05:
	#ifdef DEV_VERSION
		  iprintf("VBlankIntrWait: 0x%08X 0x%08X\n",REG_IE,anytimejmpfilter);
		  //VblankHandler(); //todo
	#endif
		//if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
#ifdef powerpatches
		if((REG_DISPSTAT & DISP_IN_VBLANK) || (REG_VCOUNT < 60))frameasyncsync(); //hope it don't need more than 100 Lines this give the emulator more power
#endif
		//while(!(REG_DISPSTAT & DISP_IN_VBLANK));
#ifndef sounddebugeraddv
		//send cmd
		SendArm7Command(0x1FFFFFFB,0);	//tell the arm7
#endif
		ichflyswiWaitForVBlank();

		break;
	  case 0x06:
		BIOS_Div();
		break;
	  case 0x07:
		BIOS_DivARM();
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
	  case 0x19:{
	//#ifdef DEV_VERSION
		  //iprintf("SoundBiasSet: 0x%08x \n",(unsigned int)R[0]);
	//#endif    
		//if(reg[0].I) //ichfly sound todo
		  //systemSoundPause(); //ichfly sound todo
		//else //ichfly sound todo
		  //systemSoundResume(); //ichfly sound todo
		}
		break;
	  case 0x1F:
		BIOS_MidiKey2Freq();
		break;
	  case 0x2A:
		BIOS_SndDriverJmpTableCopy();
		break;
		// let it go, because we don't really emulate this function
	  case 0x2D: //debug call all
		debugDump();
		break;
	  case 0x2F: //debug call all break
		//iprintf("irqBank %08x",readbankedsp(0x12));
		  debugDump();

		REG_IME = IME_DISABLE;
		while(1);
		break;
	  default:
		if((comment & 0x30) == 0x30)
		{
			iprintf("r%x %08x",(unsigned int)(comment & 0xF),(unsigned int)R[(comment & 0x30)]);
		}
		else
		{
			if(!disableMessage) {
			  systemMessage(MSG_UNSUPPORTED_BIOS_FUNCTION,
							N_("Unsupported BIOS function %02x. A BIOS file is needed in order to get correct behaviour."),
							(unsigned int)comment);
			  disableMessage = true;
			}
		}
		break;
	  }
}
void switch_to_unprivileged_mode()
{
	u32 temp = cpuGetCPSR();
	temp = temp & ~0x1F;
	temp |= 0x10;
	cpuSetCPSR(temp);
}

void emulateedbiosstart()
{
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() &~BIT(13));
}

__attribute__((section(".itcm")))
void ARMV5toARMV4Mode()	//aka downreadcpu
{
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() | BIT(15));
}

__attribute__((section(".itcm")))
void ARMV4toARMV5Mode()
{
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() &~ BIT(15));
}


inline void puGba()
{
	pu_SetCodePermissions(0x06333333);
	pu_SetDataPermissions(0x06333333);	
}
inline void puNds()
{
	pu_SetDataPermissions(0x33333333);
	pu_SetCodePermissions(0x33333333);
}


#ifndef releas
void ndsExceptionHdl()
{
	u32 mode = cpuGetCPSR() & 0x1F;
	u32 instrset = BIOSDBG_SPSR & 0x20;
	if(mode == 0x17)
	{
		u32 codeAddress = exRegs[15] - 8;
		if (	(codeAddress > 0x02000000 && codeAddress < 0x02400000) ||
		(codeAddress > (u32)0x01000000 && codeAddress < (u32)(0x01000000 + 32768)) )
		{
			iprintf("NDS DATA ABORT AT %08X\n",(unsigned int)getExceptionAddress( codeAddress, instrset));
		}
		else
		{
			iprintf("NDS DATA ABORT\n");
		}
	}
	else if(mode == 0x1B) iprintf("NDS UNDEFINED INSTRUCTION\n");
	else iprintf("NDS STRANGE EXCEPTION !\n");
	iprintf("SAVED PC = %08X (%s)\n", (unsigned int)exRegs[15], instrset ? "THUMB" : "ARM");
	debugDump();
	/*if(instrset) iprintf("FAILED INSTR = %04X\n", *(u16*)(exRegs[15] - (mode == 0x17 ? 4 : 2)));
	else iprintf("FAILED INSTR = %08X\n", *(u32*)(exRegs[15] - (mode == 0x17 ? 8 : 4)));*/ //ichfly don't like that
			REG_IME = IME_DISABLE;
		while(1);
}
#endif

inline void ndsModeinline()
{
	puNds();
#ifndef releas
	exInit(ndsExceptionHdl);
#endif
}

void ndsMode()
{
	puNds();
#ifndef releas
	exInit(ndsExceptionHdl);
#endif
}


void gbaExceptionHdl()
{

	u32 instr;
	u32 cpuMode;
	
	//ndsModeinline();
	cpuMode = BIOSDBG_SPSR & 0x20;
	//iprintf("%08X\n",exRegs[15]);
	
	//iprintf("enter\n");

#ifndef gba_handel_IRQ_correct
	BIOSDBG_SPSR = BIOSDBG_SPSR & ~0x80; //sorry but this must be done
#endif

	//iprintf("%08X %08X %08X\r\n",exRegs[15],REG_VCOUNT,REG_IE);
	/*int i;
	for(i = 0; i <= 15; i++) {
		iprintf("R%d=%X ", i, exRegs[i]);
	} 
	iprintf("\n");*/


#ifndef unsave
	if(exRegs[15] < 0x02000000 
#ifdef checkclearaddr20
	|| (exRegs[15] > 0x023FFFFF && exRegs[15] < 0x03000000 ) 
#endif
#ifdef checkclearaddr
	|| exRegs[15] > 0x03007FFF)
#else
	|| exRegs[15] > 0x04000000)
#endif
	{

		/*if(exRegs[15] > 0x08000000)//don't know why this land herer but it dose
		{
			exRegs[15] = (exRegs[15] & 0x01FFFFFF) + (s32)rom;
		}
		else*/
		{
			iprintf("gba jumped to an unknown region\n");
			debugDump(); //test
					REG_IME = IME_DISABLE;
		while(1);
		}
	}
#endif

	if(cpuMode)
	{
		instr = *(u16*)(exRegs[15] - 8);
		exRegs[15] -= 2;
		{
			emuInstrTHUMB(instr, exRegs);
		}		
	}
	else
	{
		instr = *(u32*)(exRegs[15] - 8);
		emuInstrARM(instr, exRegs);
	}

#ifdef lastdebug
	if(readbankedsp(0x12) < 0x1000000)debugandhalt();
	lasttime[lastdebugcurrent] = exRegs[15];
	lastdebugcurrent++;
	if(lastdebugcurrent == lastdebugsize)lastdebugcurrent = 0;
#endif
	//gbaMode();
	//iprintf("exit\n");
}


#ifndef releas
void gbaMode()
{

	exInit(gbaExceptionHdl);
	puGba();
	
}
 void gbaMode2()
{
	exInit(gbaExceptionHdl);
	puGba();	
}
#endif

 #ifdef releas

void gbaMode2()
{
	puGba();	
}
inline void gbaMode()
{
	puGba();	
}
#endif

//---------------------------------------------------------------------------------
u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState) {
//---------------------------------------------------------------------------------

	int Rf, Rb, Rd, Rn, Rm;

	if (thumbState) {
		// Thumb

		unsigned short opcode = *(unsigned short *)opcodeAddress ;
		// ldr r,[pc,###]			01001ddd ffffffff
		// ldr r,[r,r]				0101xx0f ffbbbddd
		// ldrsh					0101xx1f ffbbbddd
		// ldr r,[r,imm]			011xxfff ffbbbddd
		// ldrh						1000xfff ffbbbddd
		// ldr r,[sp,###]			1001xddd ffffffff
		// push						1011x10l llllllll
		// ldm						1100xbbb llllllll


		if ((opcode & 0xF800) == 0x4800) {
			// ldr r,[pc,###]
			s8 offset = opcode & 0xff;
			return exRegs[15] + offset;
		} else if ((opcode & 0xF200) == 0x5000) {
			// ldr r,[r,r]
			Rb = (opcode >> 3) & 0x07 ;
			Rf = (opcode >> 6) & 0x07 ;
			return exRegs[Rb] + exRegs[Rf];

		} else if ((opcode & 0xF200) == 0x5200) {
			// ldrsh
			Rb = (opcode >> 3) & 0x07;
			Rf = (opcode >> 6) & 0x03;
			return exRegs[Rb] + exRegs[Rf];

		} else if ((opcode & 0xE000) == 0x6000) {
			// ldr r,[r,imm]
			Rb = (opcode >> 3) & 0x07;
			Rf = (opcode >> 6) & 0x1F ;
			return exRegs[Rb] + (Rf << 2);
		} else if ((opcode & 0xF000) == 0x8000) {
			// ldrh
			Rb = (opcode >> 3) & 0x07 ;
			Rf = (opcode >> 6) & 0x1F ;
			return exRegs[Rb] + (Rf << 2);
		} else if ((opcode & 0xF000) == 0x9000) {
			// ldr r,[sp,#imm]
			s8 offset = opcode & 0xff;
			return exRegs[13] + offset;
		} else if ((opcode & 0xF700) == 0xB500) {
			// push/pop
			return exRegs[13];
		} else if ((opcode & 0xF000) == 0xC000) {
			// ldm/stm
			Rd = (opcode >> 8) & 0x07;
			return exRegs[Rd];
		}
	} else {
		// arm32
		unsigned long opcode = *(unsigned long *)opcodeAddress ;

		// SWP			xxxx0001 0x00nnnn dddd0000 1001mmmm
		// STR/LDR		xxxx01xx xxxxnnnn ddddffff ffffffff
		// STRH/LDRH	xxxx000x x0xxnnnn dddd0000 1xx1mmmm
		// STRH/LDRH	xxxx000x x1xxnnnn ddddffff 1xx1ffff
		// STM/LDM		xxxx100x xxxxnnnn llllllll llllllll

		if ((opcode & 0x0FB00FF0) == 0x01000090) {
			// SWP
			Rn = (opcode >> 16) & 0x0F;
			return exRegs[Rn];
		} else if ((opcode & 0x0C000000) == 0x04000000) {
			// STR/LDR
			Rn = (opcode >> 16) & 0x0F;
			if (opcode & 0x02000000) {
				// Register offset
				Rm = opcode & 0x0F;
				if (opcode & 0x01000000) {
					unsigned short shift = (unsigned short)((opcode >> 4) & 0xFF) ;
					// pre indexing
					long Offset = ARMShift(exRegs[Rm],shift);
					// add or sub the offset depending on the U-Bit
					return exRegs[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
				} else {
					// post indexing
					return exRegs[Rn];
				}
			} else {
				// Immediate offset
				unsigned long Offset = (opcode & 0xFFF) ;
				if (opcode & 0x01000000) {
					// pre indexing
					// add or sub the offset depending on the U-Bit
					return exRegs[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
				} else {
					// post indexing
					return exRegs[Rn];
				}
			}
		} else if ((opcode & 0x0E400F90) == 0x00000090) {
			// LDRH/STRH with register Rm
			Rn = (opcode >> 16) & 0x0F;
			Rd = (opcode >> 12) & 0x0F;
			Rm = opcode & 0x0F;
			unsigned short shift = (unsigned short)((opcode >> 4) & 0xFF);
			long Offset = ARMShift(exRegs[Rm],shift);
			// add or sub the offset depending on the U-Bit
			return exRegs[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
		} else if ((opcode & 0x0E400F90) == 0x00400090) {
			// LDRH/STRH with immediate offset
			Rn = (opcode >> 16) & 0x0F;
			Rd = (opcode >> 12) & 0x0F;
			unsigned long Offset = (opcode & 0xF) | ((opcode & 0xF00)>>8) ;
			// add or sub the offset depending on the U-Bit
			return exRegs[Rn] + ((opcode & 0x00800000)?Offset:-Offset) ;
		} else if ((opcode & 0x0E000000) == 0x08000000) {
			// LDM/STM
			Rn = (opcode >> 16) & 0x0F;
			return exRegs[Rn];
		}
	}
	return 0;
}


//---------------------------------------------------------------------------------
unsigned long ARMShift(unsigned long value,unsigned char shift) {
//---------------------------------------------------------------------------------
	// no shift at all
	if (shift == 0x0B) return value ;
	int index ;
	if (shift & 0x01) {
		// shift index is a register
		index = exRegs[(shift >> 4) & 0x0F];
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

void debugme(){
	REG_IME = IME_DISABLE;
	debugDump();
	while(1);
}
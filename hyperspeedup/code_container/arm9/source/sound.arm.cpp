#include <nds.h>
#include <stdio.h>
#include "ichflysettings.h"
#include "main.h"

















#include <nds.h>
#include <stdio.h>


#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include <dirent.h>

#include "cpumg.h"
#include "GBAinline.h"
#include "bios.h"

#include "mydebuger.h"

#include "ichflysettings.h"

#include <nds.h>

#include "arm7sound.h"

#include "main.h"

#define UPDATE_REG(address, value)\
  {\
    WRITE16LE(((u16 *)&ioMem[address]),value);\
  }\

extern char savePath[MAXPATHLEN * 2];

extern char szFile[MAXPATHLEN * 2];

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




























#include "Cheats.h"

extern "C" int SPtoload;
extern "C" int SPtemp;

int cheatsCheckKeys();

int counttrans = 0;

int dasistnurzumtesten = 0;

extern u32 arm7amr9buffer;

void arm7dmareq()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//counttrans++;
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		int i = 0;
		u32* src = (u32*)REG_IPC_FIFO_RX;
#ifdef unsecamr7com
		if(src < (u32*)0x10000000)		
#else
		if(src < (u32*)0x8000000 && src > (u32*)0x2000000)
#endif
		{
			//iprintf("%08X\r\n",REG_IPC_FIFO_RX);
			//iprintf("%08X %08X\n\r",src,REG_IPC_FIFO_CR);
			if(counttrans < (u32)src)counttrans = (u32)src;
			REG_IPC_FIFO_TX = 0x1;
			while(i < 4)
			{
				REG_IPC_FIFO_TX = *src;
				//REG_IPC_FIFO_TX = 0;
				src++;
				i++;
			}
		}
		else
		{
			if(src == (u32*)0x3F00BEEF)
			{
				VblankHandler();
				dasistnurzumtesten++;
				if(dasistnurzumtesten == 60)
				{
					iprintf("alive %08X\r\n",counttrans);
					dasistnurzumtesten = 0;
					counttrans = 0;
				}
				//counttrans = 0;
				continue;
			}
			if(src == (u32*)0x4000BEEF)
			{
				while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
				iprintf("arm7 %08X\r\n",REG_IPC_FIFO_RX);
				continue;
			}
			if(src == (u32*)0x4100BEEF)
			{
				frameasyncsync();
				continue;
			}
			if(src == (u32*)0x4200BEEF)
			{
				if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
				CPUWriteBatteryFile(savePath);
				REG_IPC_FIFO_TX = 0;
				continue;
			}
			iprintf("error rec %08X %08X\r\n",src,REG_IPC_FIFO_CR);
		}
		//iprintf("e %08X\r\n",REG_IPC_FIFO_CR);
	}

 }
void arm7dmareqandcheat()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		int i = 0;
		u32* src = (u32*)REG_IPC_FIFO_RX;
		//iprintf("i %08X\r\n",src);
		if(src < (u32*)0x10000000)
		{
			//iprintf("%08X\r\n",REG_IPC_FIFO_RX);
			//iprintf("%08X %08X\n\r",src,REG_IPC_FIFO_CR);
			while(i < 4)
			{
				REG_IPC_FIFO_TX = *src;
				src++;
				i++;
			}
		}
		else
		{
			if(src == (u32*)0x3F00BEEF)
			{
				cheatsCheckKeys();
				VblankHandler();
			}
			if(src == (u32*)0x4100BEEF)
			{
				frameasyncsync();
			}
			if(src == (u32*)0x4200BEEF)
			{
				if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
				CPUWriteBatteryFile(savePath);
				REG_IPC_FIFO_TX = 0;
			}
		}
		//if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))arm7dmareqandcheat();
	}

 }
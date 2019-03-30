#include <nds.h>
#include <nds/ndstypes.h>
#include <nds/memory.h>

#include <stdio.h>
#include <fat.h>
#include <dirent.h>
#include <filesystem.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>

#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include "cpumg.h"
#include "bios.h"
#include "mydebuger.h"
#include "file_browse.h"
#include "main.h"
#include "ds_dma.h"

#define readanom 0x100

char* seloptionsshowmem [6] = {(char*)"dump ram",(char*)"dump gba ram",(char*)"show nds ram",(char*)"show gba ram",(char*)"cram dump",(char*)"exit"};

u32 userinputval(u32 original_val,u32 bits)
{
	while(1)
	{
		u32 pressed;
		u32 srctempmulti = 0x10;
		do {
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			scanKeys();
			pressed = (keysDownRepeat()& ~0xFC00);
		} while (!pressed);
		iprintf("\x1b[2J");
		iprintf("val %08X multi: %08X\n",(unsigned int)original_val,(unsigned int)srctempmulti);

		if (pressed&KEY_UP) original_val+= srctempmulti;
		if (pressed&KEY_DOWN && original_val != 0) original_val-=srctempmulti;
		if (pressed&KEY_RIGHT) srctempmulti *= 2;
		if (pressed&KEY_LEFT && srctempmulti != 1) srctempmulti /= 2;
		if (pressed&KEY_SELECT) break;
	}
	return original_val;
}

void show_gba_mem()
{
	u32 pressed=0;
	u32 src = 0x03000000;
	u32 srctempmulti = 0x10000;
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu
	u8 type = 1;
	u8 writeon = 0;
	u16 pointer = 0;
	while(1) 
	{
		iprintf("gba src %08X multi: %08X\n",(unsigned int)src,(unsigned int)srctempmulti);
		switch (type)
		{
			case 1:
			{
				for(int i = 0;i < readanom;i++)
				{
					if(writeon && pointer == i)
					{
						iprintf("\x1b[33[0m%02X\x1b[39[0m",(unsigned int)CPUReadBytereal(src + i));
					}
					else
					{
						iprintf("%02X",(unsigned int)CPUReadBytereal(src + i));
					}
				}
			}
			if (pressed&KEY_A)
			{
				if(writeon)
				{
					CPUWriteByte(src + pointer,(u8)userinputval(CPUReadBytereal(src + pointer),type));
				}
				else
				{
					type= 2;
				}
			}
			iprintf("Byte mode");
			break;
			case 2:
			{
				for(int i = 0;i < readanom;i+=2)
				{
					if(writeon && pointer == i)
					{
						iprintf("\x1b[33[0m%04X\x1b[39[0m",CPUReadHalfWordreal(src + i));
					}
					else
					{
						iprintf("%04X",CPUReadHalfWordreal(src + i));
					}
				}
			}
			if (pressed&KEY_A)
			{
				if(writeon)
				{
					CPUWriteHalfWord(src + pointer,(u16)userinputval(CPUReadHalfWordreal(src + pointer),type));
				}
				else
				{
					type= 4;
				}
			}
			iprintf("Hword mode");
			break;
			case 4:
			{
				for(int i = 0;i < readanom;i+=4)
				{
					if(writeon && pointer == i)
					{
						iprintf("\x1b[33[0m%08X\x1b[39[0m",(unsigned int)CPUReadMemoryreal(src + i));
					}
					else
					{
						iprintf("%08X",(unsigned int)CPUReadMemoryreal(src + i));
					}
				}
			}
			iprintf("Word mode");
			if (pressed&KEY_A)
			{
				if(writeon)
				{
					CPUWriteMemory(src + pointer,(u32)userinputval(CPUReadMemoryreal(src + pointer),type));
				}
				else
				{
					type= 1;
				}
			}
		}
		do {
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			scanKeys();
			pressed = (keysDownRepeat()& ~0xFC00);
		} while (!pressed);
		if(writeon)
		{
			if (pressed&KEY_UP && src != 0) pointer-= 0x10;
			if (pressed&KEY_DOWN) pointer+= 0x10;
			if (pressed&KEY_RIGHT) pointer++;
			if (pressed&KEY_LEFT && src != 0) pointer--;
		}
		else
		{
			if (pressed&KEY_UP) src+= srctempmulti;
			if (pressed&KEY_DOWN && src != 0) src-=srctempmulti;
			if (pressed&KEY_RIGHT) srctempmulti *= 2;
			if (pressed&KEY_LEFT && srctempmulti != 1) srctempmulti /= 2;
		}
		if (pressed&KEY_START) break;
		if (pressed&KEY_SELECT) writeon = writeon ? 0 : 1;
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		iprintf("\x1b[2J");
	}
	pu_Enable(); //back to normal code
}


void show_nds_mem() //change colour with \x1b[33[0m back with \x1b[39[0m
{
	u32 pressed=0;
	u32 src = 0x03000000;
	u32 srctempmulti = 0x10000;
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu
	u8 type = 1;
	u8 writeon = 0;
	u16 pointer = 0;
	while(1) 
	{
		iprintf("nds src %08X multi: %08X\n",(unsigned int)src,(unsigned int)srctempmulti);
		switch (type)
		{
			case 1:
			{
				for(int i = 0;i < readanom;i++)
				{
					if(writeon && pointer == i)
					{
						iprintf("\x1b[33[0m%02X\x1b[39[0m",*(u8*)(src + i));
					}
					else
					{
						iprintf("%02X",*(u8*)(src + i));
					}
				}
			}
			if (pressed&KEY_A)
			{
				if(writeon)
				{
					*(u8*)(src + pointer) = (u8)userinputval(*(u8*)(src + pointer),type);
				}
				else
				{
					type= 2;
				}
			}
			iprintf("Byte mode");
			break;
			case 2:
			{
				for(int i = 0;i < readanom;i+=2)
				{
					if(writeon && pointer == i)
					{
						iprintf("\x1b[33[0m%04X\x1b[39[0m",*(u16*)(src + i));
					}
					else
					{
						iprintf("%04X",*(u16*)(src + i));
					}
				}
			}
			if (pressed&KEY_A)
			{
				if(writeon)
				{
					*(u16*)(src + pointer) =(u16)userinputval(*(u16*)(src + pointer),type);
				}
				else
				{
					type= 4;
				}
			}
			iprintf("Hword mode");
			break;
			case 4:
			{
				for(int i = 0;i < readanom;i+=4)
				{
					if(writeon && pointer == i)
					{
						iprintf("\x1b[33[0m%08X\x1b[39[0m",(unsigned int)*(u32*)(src + i));
					}
					else
					{
						iprintf("%08X",(unsigned int)*(u32*)(src + i));
					}
				}
			}
			iprintf("Word mode");
			if (pressed&KEY_A)
			{
				if(writeon)
				{
					*(u32*)(src + pointer) = (u32)userinputval(CPUReadMemoryreal(src + pointer),type);
				}
				else
				{
					type= 1;
				}
			}
		}
		do {
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			scanKeys();
			pressed = (keysDownRepeat()& ~0xFC00);
		} while (!pressed);
		if(writeon)
		{
			if (pressed&KEY_UP && src != 0) pointer-= 0x10;
			if (pressed&KEY_DOWN) pointer+= 0x10;
			if (pressed&KEY_RIGHT) pointer++;
			if (pressed&KEY_LEFT && src != 0) pointer--;
		}
		else
		{
			if (pressed&KEY_UP) src+= srctempmulti;
			if (pressed&KEY_DOWN && src != 0) src-=srctempmulti;
			if (pressed&KEY_RIGHT) srctempmulti *= 2;
			if (pressed&KEY_LEFT && srctempmulti != 1) srctempmulti /= 2;
		}
		if (pressed&KEY_START) break;
		if (pressed&KEY_SELECT) writeon = writeon ? 0 : 1;
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		iprintf("\x1b[2J");
	}
	pu_Enable(); //back to normal code
}

void show_mem()
{
	int pressed;
	int ausgewauhlt = 0;
	while(1)
	{
		iprintf("\x1b[2J");
		iprintf("show mem\n");
		iprintf ("--------------------------------");
		for(int i = 0; i < 6; i++)
		{
			if(i == ausgewauhlt) iprintf("->");
			else iprintf("  ");
			iprintf(seloptionsshowmem[i]);
			iprintf("\n");
		}
		do {
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			scanKeys();
			pressed = (keysDownRepeat()& ~0xFC00);
		} while (!pressed); //no communication here with arm7 so no more update
		//iprintf("%x",ausgewauhlt);
		FILE *file;
		if (pressed&KEY_A)
		{
			switch(ausgewauhlt)
				{
				case 0: //dump ram
					file = fopen("fat:/fulldump.bin", "wb"); //4.411.976 Byte
					fwrite((u8*)(0x01000000), 1, 0x8000, file);
					fwrite((u8*)(0x03000000), 1, 0x8000, file);
					fwrite((u8*)(0x04000000), 1, 0x400, file);//IO
					fwrite((u8*)(0x05000000), 1, 0x800, file);
					fwrite((u8*)(0x06000000), 1, 0x20000, file);
					fwrite((u8*)(0x07000000), 1, 0x800, file);
					fwrite((u8*)(0x0b000000), 1, 0x4000, file);
					fwrite((u8*)(0x02000000), 1, 0x400000, file);
					fclose(file);
					break;
				case 1:
					file = fopen("fat:/gbadump.bin", "wb"); // 396.288 Byte @outdate
					fwrite((u8*)(0x03000000), 1, 0x8000, file);
					fwrite(ioMem, 1, 0x400, file);
					fwrite((u8*)(0x04000000), 1, 0x400, file);//IO
					fwrite((u8*)(0x05000000), 1, 0x400, file);
					fwrite((u8*)(0x07000000), 1, 0x800, file);
					fwrite((u8*)(0x01000000), 1, 0x8000, file);
					fwrite((u8*)(0x0b000000), 1, 0x4000, file);
					fwrite((u8*)(0x06000000), 1, 0x18000, file); //can't get this with half dumps
					fwrite((u8*)(0x02000000), 1, 0x40000, file); //can't get this with half dumps
					fclose(file);
					break;
				case 2:
					show_nds_mem();
					break;
				case 3:
					show_gba_mem();
					break;
				case 4:
					{
							int src = 0x023F0000; 
							int srctempmulti = 0x80000;
							while(1) {		
								iprintf("src %08X multi: %08X\n",src,srctempmulti);
								if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
								while(!(REG_DISPSTAT & DISP_IN_VBLANK));
								scanKeys();		
								if (keysDown()&KEY_START) break;
								if (keysDown()&KEY_UP) src+= srctempmulti;
								if (keysDown()&KEY_DOWN && src != 0) src-=srctempmulti;
								if (keysDown()&KEY_RIGHT) srctempmulti *= 2;
								if (keysDown()&KEY_LEFT && srctempmulti != 1) srctempmulti /= 2;
								iprintf("\x1b[2J");
							}
							int size = 0x10000; 
							int sizetempmulti = 0x10000;
							while(1) {		
								iprintf("size %08X multi: %08X\n",size,sizetempmulti);
								scanKeys();		
								if (keysDown()&KEY_A) break;
								if (keysDown()&KEY_UP) size+= sizetempmulti;
								if (keysDown()&KEY_DOWN && size != 0) size-=sizetempmulti;
								if (keysDown()&KEY_RIGHT) sizetempmulti *= 2;
								if (keysDown()&KEY_LEFT && sizetempmulti != 1) sizetempmulti /= 2;
								if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
								while(!(REG_DISPSTAT & DISP_IN_VBLANK));
								iprintf("\x1b[2J");
							}
							iprintf("src %08X size: %X\n",src,size);
							while(1) {
								scanKeys();
								if (keysDown()&KEY_START) break;
								if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
								while(!(REG_DISPSTAT & DISP_IN_VBLANK));
							}
							file = fopen("fat:/cram_dump.bin", "wb");
							fwrite((u8*)(src), 1, size, file);
							fclose(file);
					}
					break;
				case 5:
					return; //and return
				}
		}
		if (pressed&KEY_DOWN && ausgewauhlt != 5){ ausgewauhlt++;}
		if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--;}

	}
}

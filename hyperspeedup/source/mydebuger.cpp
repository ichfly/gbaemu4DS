#include <nds.h>
#include <stdio.h>

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
#include "bios.h"

#include "mydebuger.h"

#include "file_browse.h"

#define MAXPATHLEN 256 

#include <nds.h>


#include "main.h"


typedef struct
{
	u32 entryPoint;
	u8 logo[156];
	char title[0xC];
	char gamecode[0x4];
	u16 makercode;
	u8 is96h;
	u8 unitcode;
	u8 devicecode;
	u8 unused[7];
	u8 version;
	u8 complement;
	u16 checksum;
} __attribute__ ((__packed__)) gbaHeader_t;

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




char* seloptionsshowmem [6] = {"dump ram","dump gba ram","show nds ram(todo)","show gba ram(todo)","cram dump","exit"};

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
					fwrite((u8*)(0x02000000), 1, 0x400000, file);
					fwrite((u8*)(0x03000000), 1, 0x8000, file);
					fwrite((u8*)(0x04000000), 1, 0x248, file);//IO
					fwrite((u8*)(0x05000000), 1, 0x800, file);
					fwrite((u8*)(0x06000000), 1, 0x20000, file);
					fwrite((u8*)(0x07000000), 1, 0x800, file);
					fwrite((u8*)(0x0b000000), 1, 0x4000, file);
					fclose(file);
					break;
				case 1:
					file = fopen("fat:/gbadump.bin", "wb"); // 396.288 Byte
					fwrite((u8*)(0x02000000), 1, 0x40000, file);
					fwrite((u8*)(0x03000000), 1, 0x8000, file);
					fwrite(ioMem, 1, 0x400, file);
					fwrite((u8*)(0x05000000), 1, 0x400, file);
					fwrite((u8*)(0x06000000), 1, 0x18000, file);
					fwrite((u8*)(0x07000000), 1, 0x800, file);
					fclose(file);
					break;
				case 2:
					break;
				case 3:
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
								iprintf("\x1b[2J");
								scanKeys();		
								if (keysDown()&KEY_START) break;
								if (keysDown()&KEY_UP) size+= sizetempmulti;
								if (keysDown()&KEY_DOWN && size != 0) size-=sizetempmulti;
								if (keysDown()&KEY_RIGHT) sizetempmulti *= 2;
								if (keysDown()&KEY_LEFT && sizetempmulti != 1) sizetempmulti /= 2;
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
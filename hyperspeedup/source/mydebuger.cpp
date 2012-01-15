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




char* seloptionsshowmem [5] = {"dump ram","dump gba ram","show nds ram(todo)","show gba ram(todo)","exit"};

void show_mem()
{
	int pressed;
	int ausgewauhlt = 0;
	while(1)
	{
		iprintf("\x1b[2J");
		iprintf("show mem\n");
		iprintf ("--------------------------------");
		for(int i = 0; i < 5; i++)
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
					return; //and return
				}
		}
		if (pressed&KEY_DOWN && ausgewauhlt != 4){ ausgewauhlt++;}
		if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--;}

	}
}
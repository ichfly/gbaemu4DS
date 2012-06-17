/*---------------------------------------------------------------------------------

	Basic template code for starting a GBA app

---------------------------------------------------------------------------------*/
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

#include "file_browse.h"

#define MAXPATHLEN 256 

#include <nds.h>

#include "arm7sound.h"

#include "main.h"

#define UPDATE_REG(address, value)\
  {\
    WRITE16LE(((u16 *)&ioMem[address]),value);\
  }\

char biosPath[MAXPATHLEN * 2];

char savePath[MAXPATHLEN * 2];

char szFile[MAXPATHLEN * 2];

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



#include <nds/arm9/dldi.h>

// The only built in driver
extern DLDI_INTERFACE _io_dldi_stub;



//#define loaddirect

void emulateedbiosstart();

volatile u16 DISPCNT;


void downgreadcpu();

//volatile u16 DISPCNT  = 0x0080;


int framenummer;


#define GBA_EWRAM ((void*)(0x02000000))

#include <nds/disc_io.h>
#include <dirent.h>

   #define DEFAULT_CACHE_PAGES 16
   #define DEFAULT_SECTORS_PAGE 8

#define public

char* rootdirnames [3] = {"nitro:/","fat:/","sd:/"};
bool rootenabelde[3];

int ausgewauhlt = 0;

char* currentdir =  (char*)0;

int seite = 0;

int dirfeldsize = 0;
char** names; //stupid i know but i don't know a better way
u32* dirfeldsizes;


int bg = 0;


char* savetypeschar[7] =
	{"SaveTypeAutomatic","SaveTypeEeprom","SaveTypeSram","SaveTypeFlash64KB","SaveTypeEepromSensor","SaveTypeNone","SaveTypeFlash128KB"};

char* memoryWaitrealram[8] =
  { "10 and 6","8 and 6","6 and 6","18 and 6","10 and 4","8 and 4","6 and 4","18 and 4" };





extern "C" void IntrMain();


extern "C" void testasm(u32* feld);
extern "C" void cpu_SetCP15Cnt(u32 v);
extern "C" u32 cpu_GetCP15Cnt();
extern "C" u32 pu_Enable();

int ignorenextY = 0;






#define READ16LE(x) \
  swap16(*((u16 *)(x)))


/*
 LCD VRAM Overview

The GBA contains 96 Kbytes VRAM built-in, located at address 06000000-06017FFF, depending on the BG Mode used as follows:

BG Mode 0,1,2 (Tile/Map based Modes)

  06000000-0600FFFF  64 KBytes shared for BG Map and Tiles
  06010000-06017FFF  32 KBytes OBJ Tiles

The shared 64K area can be split into BG Map area(s), and BG Tiles area(s), the respective addresses for Map and Tile areas are set up by BG0CNT-BG3CNT registers. The Map address may be specified in units of 2K (steps of 800h), the Tile address in units of 16K (steps of 4000h).

BG Mode 0,1 (Tile/Map based Text mode)
The tiles may have 4bit or 8bit color depth, minimum map size is 32x32 tiles, maximum is 64x64 tiles, up to 1024 tiles can be used per map.

  Item        Depth     Required Memory
  One Tile    4bit      20h bytes
  One Tile    8bit      40h bytes
  1024 Tiles  4bit      8000h (32K)
  1024 Tiles  8bit      10000h (64K) - excluding some bytes for BG map
  BG Map      32x32     800h (2K)
  BG Map      64x64     2000h (8K)


BG Mode 1,2 (Tile/Map based Rotation/Scaling mode)
The tiles may have 8bit color depth only, minimum map size is 16x16 tiles, maximum is 128x128 tiles, up to 256 tiles can be used per map.

  Item        Depth     Required Memory
  One Tile    8bit      40h bytes
  256  Tiles  8bit      4000h (16K)
  BG Map      16x16     100h bytes
  BG Map      128x128   4000h (16K)


BG Mode 3 (Bitmap based Mode for still images)

  06000000-06013FFF  80 KBytes Frame 0 buffer (only 75K actually used)
  06014000-06017FFF  16 KBytes OBJ Tiles


BG Mode 4,5 (Bitmap based Modes)

  06000000-06009FFF  40 KBytes Frame 0 buffer (only 37.5K used in Mode 4)
  0600A000-06013FFF  40 KBytes Frame 1 buffer (only 37.5K used in Mode 4)
  06014000-06017FFF  16 KBytes OBJ Tiles


Note
Additionally to the above VRAM, the GBA also contains 1 KByte Palette RAM (at 05000000h) and 1 KByte OAM (at 07000000h) which are both used by the display controller as well.


 LCD VRAM Character Data

Each character (tile) consists of 8x8 dots (64 dots in total). The color depth may be either 4bit or 8bit (see BG0CNT-BG3CNT).

4bit depth (16 colors, 16 palettes)
Each tile occupies 32 bytes of memory, the first 4 bytes for the topmost row of the tile, and so on. Each byte representing two dots, the lower 4 bits define the color for the left (!) dot, the upper 4 bits the color for the right dot.

8bit depth (256 colors, 1 palette)
Each tile occupies 64 bytes of memory, the first 8 bytes for the topmost row of the tile, and so on. Each byte selects the palette entry for each dot.


 LCD VRAM BG Screen Data Format (BG Map)

The display background consists of 8x8 dot tiles, the arrangement of these tiles is specified by the BG Screen Data (BG Map). The separate entries in this map are as follows:

Text BG Screen (2 bytes per entry)
Specifies the tile number and attributes. Note that BG tile numbers are always specified in steps of 1 (unlike OBJ tile numbers which are using steps of two in 256 color/1 palette mode).

  Bit   Expl.
  0-9   Tile Number     (0-1023) (a bit less in 256 color mode, because
                           there'd be otherwise no room for the bg map)
  10    Horizontal Flip (0=Normal, 1=Mirrored)
  11    Vertical Flip   (0=Normal, 1=Mirrored)
  12-15 Palette Number  (0-15)    (Not used in 256 color/1 palette mode)

A Text BG Map always consists of 32x32 entries (256x256 pixels), 400h entries = 800h bytes. However, depending on the BG Size, one, two, or four of these Maps may be used together, allowing to create backgrounds of 256x256, 512x256, 256x512, or 512x512 pixels, if so, the first map (SC0) is located at base+0, the next map (SC1) at base+800h, and so on.

Rotation/Scaling BG Screen (1 byte per entry)
In this mode, only 256 tiles can be used. There are no x/y-flip attributes, the color depth is always 256 colors/1 palette.

  Bit   Expl.
  0-7   Tile Number     (0-255)

The dimensions of Rotation/Scaling BG Maps depend on the BG size. For size 0-3 that are: 16x16 tiles (128x128 pixels), 32x32 tiles (256x256 pixels), 64x64 tiles (512x512 pixels), or 128x128 tiles (1024x1024 pixels).

The size and VRAM base address of the separate BG maps for BG0-3 are set up by BG0CNT-BG3CNT registers.


 LCD VRAM Bitmap BG Modes

In BG Modes 3-5 the background is defined in form of a bitmap (unlike as for Tile/Map based BG modes). Bitmaps are implemented as BG2, with Rotation/Scaling support. As bitmap modes are occupying 80KBytes of BG memory, only 16KBytes of VRAM can be used for OBJ tiles.

BG Mode 3 - 240x160 pixels, 32768 colors
Two bytes are associated to each pixel, directly defining one of the 32768 colors (without using palette data, and thus not supporting a 'transparent' BG color).

  Bit   Expl.
  0-4   Red Intensity   (0-31)
  5-9   Green Intensity (0-31)
  10-14 Blue Intensity  (0-31)
  15    Not used

The first 480 bytes define the topmost line, the next 480 the next line, and so on. The background occupies 75 KBytes (06000000-06012BFF), most of the 80 Kbytes BG area, not allowing to redraw an invisible second frame in background, so this mode is mostly recommended for still images only.

BG Mode 4 - 240x160 pixels, 256 colors (out of 32768 colors)
One byte is associated to each pixel, selecting one of the 256 palette entries. Color 0 (backdrop) is transparent, and OBJs may be displayed behind the bitmap.
The first 240 bytes define the topmost line, the next 240 the next line, and so on. The background occupies 37.5 KBytes, allowing two frames to be used (06000000-060095FF for Frame 0, and 0600A000-060135FF for Frame 1).

BG Mode 5 - 160x128 pixels, 32768 colors
Colors are defined as for Mode 3 (see above), but horizontal and vertical size are cut down to 160x128 pixels only - smaller than the physical dimensions of the LCD screen.
The background occupies exactly 40 KBytes, so that BG VRAM may be split into two frames (06000000-06009FFF for Frame 0, and 0600A000-06013FFF for Frame 1).

In BG modes 4,5, one Frame may be displayed (selected by DISPCNT Bit 4), the other Frame is invisible and may be redrawn in background.
*/





int frameskip = 10;

int framewtf = 0;


int oldmode = 0;


extern "C" int SPtoload;
extern "C" int SPtemp;


//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	

#ifdef lastdebug
lasttime[lastdebugcurrent] = 0x0000000;
lastdebugcurrent++;
if(lastdebugcurrent == lastdebugsize)lastdebugcurrent = 0;
#endif

	CPUCheckDMA(1, 0x0f);	
	
	if(framewtf == frameskip)
	{
		framewtf = 0;
		if((DISPCNT & 7) < 3)
		{
			dmaCopyWordsAsynch(1,(void*)vram + 0x10000,(void*)0x06400000,0x8000);
		}
		else
		{
			dmaCopyWordsAsynch(1,(void*)0x06014000,(void*)0x06404000,0x4000);
			if((DISPCNT & 7) == 3) //BG Mode 3 - 240x160 pixels, 32768 colors
			{
				u8 *pointertobild = (u8 *)(0x6000000);
				for(int iy = 0; iy <160; iy++){
					dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+512*(iy), 480);
					pointertobild+=480;
				}
			}
			else
			{
				if((DISPCNT & 7) == 4) //BG Mode 4 - 240x160 pixels, 256 colors (out of 32768 colors)
				{
					u8 *pointertobild = (u8 *)(0x6000000);
					if(BIT(4) & DISPCNT)pointertobild+=0xA000;
					for(int iy = 0; iy <160; iy++){
						dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+256*(iy), 240);
						pointertobild+=240;
						//pointertobild+=120;
					}
				}
				else
				{
					//if((DISPCNT & 7) == 5) //BG Mode 5 - 160x128 pixels, 32768 colors //ichfly can't be other mode
					{
						u8 *pointertobild = (u8 *)(0x6000000);
						if(BIT(4) & DISPCNT)pointertobild+=0xA000;
						for(int iy = 0; iy <128; iy++){
							dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+512*(iy), 320);
							pointertobild+=320;
						}
					}
				}
			}
		}
	}
	else
	{
		framewtf++;
	}
	

	
  
    P1 = REG_KEYINPUT&0x3ff;
#ifdef ichflytestkeypossibillity  
  
  // disallow Left+Right or Up+Down of being pressed at the same time
  if((P1 & 0x30) == 0)
    P1 |= 0x10;
  if((P1 & 0xC0) == 0)
    P1 |= 0x80;
#endif
	if(!(P1 & KEY_A) && !(P1 & KEY_B) && !(P1 & KEY_R) && !(P1 & KEY_L))
	{
		if(ignorenextY == 0)
		{
			pausemenue();
			ignorenextY = 60; // 1 sec break time
		}
		else {ignorenextY -= 1;}
	}            
    UPDATE_REG(0x130, P1);


#ifdef lastdebug
lasttime[lastdebugcurrent] = 0x0000001;
lastdebugcurrent++;
if(lastdebugcurrent == lastdebugsize)lastdebugcurrent = 0;
#endif
}







void frameasyncsync(void) {
//---------------------------------------------------------------------------------
		framewtf = 0;
		if((DISPCNT & 7) < 3)
		{
			dmaCopyWordsAsynch(1,(void*)vram + 0x10000,(void*)0x06400000,0x8000);
		}
		else
		{
			dmaCopyWordsAsynch(1,(void*)0x06014000,(void*)0x06404000,0x4000);
			if((DISPCNT & 7) == 3) //BG Mode 3 - 240x160 pixels, 32768 colors
			{
				u8 *pointertobild = (u8 *)(0x6000000);
				for(int iy = 0; iy <160; iy++){
					dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+512*(iy), 480);
					pointertobild+=480;
				}
			}
			else
			{
				if((DISPCNT & 7) == 4) //BG Mode 4 - 240x160 pixels, 256 colors (out of 32768 colors)
				{
					u8 *pointertobild = (u8 *)(0x6000000);
					if(BIT(4) & DISPCNT)pointertobild+=0xA000;
					for(int iy = 0; iy <160; iy++){
						dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+256*(iy), 240);
						pointertobild+=240;
						//pointertobild+=120;
					}
				}
				else
				{
					//if((DISPCNT & 7) == 5) //BG Mode 5 - 160x128 pixels, 32768 colors //ichfly can't be other mode
					{
						u8 *pointertobild = (u8 *)(0x6000000);
						if(BIT(4) & DISPCNT)pointertobild+=0xA000;
						for(int iy = 0; iy <128; iy++){
							dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+512*(iy), 320);
							pointertobild+=320;
						}
					}
				}
			}
		}
	}







bool gbamode = false;



char* seloptions [3] = {"save save","show mem","Continue"};

void pausemenue()
{
	REG_IE = 0; //no irq
	u16 tempvcount = REG_VCOUNT;
	TIMER0_CR = TIMER0_CR & ~TIMER_ENABLE; //timer off
	TIMER1_CR = TIMER1_CR & ~TIMER_ENABLE;
	TIMER2_CR = TIMER2_CR & ~TIMER_ENABLE;
	TIMER3_CR = TIMER3_CR & ~TIMER_ENABLE;
	//irqDisable(IRQ_VBLANK);
	//cpupausemode(); //don't need that
	int pressed;
	int ausgewauhlt = 0;
	while(1)
	{
		iprintf("\x1b[2J");
		iprintf("Pause\n");
		iprintf ("--------------------------------");
		for(int i = 0; i < 3; i++)
		{
			if(i == ausgewauhlt) iprintf("->");
			else iprintf("  ");
			iprintf(seloptions[i]);
			iprintf("\n");
		}
		do {
			for(int asdlkjalksjdf = 0; asdlkjalksjdf < 60;asdlkjalksjdf++)
			{
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			}
			pressed = (~REG_KEYINPUT&0x3ff);
		} while (!pressed); //no communication here with arm7 so no more update
		//iprintf("%x",ausgewauhlt);
		if (pressed&KEY_A)
		{
			switch(ausgewauhlt)
				{
				case 0:
					if(savePath[0] == 0)sprintf(savePath,"%s.sav",szFile);
					CPUWriteBatteryFile(savePath);
					break;
				case 1:
#ifndef noichflydebugger
					show_mem();
#endif
					break;
				case 2:
					iprintf("\x1b[2J");
					while(REG_VCOUNT != tempvcount); //wait for VCOUNT
					TIMER0_CR = timer0Value; //timer on
					TIMER1_CR = timer1Value;
					TIMER2_CR = timer2Value;
					TIMER3_CR = timer3Value;
					REG_IE = IE | IRQ_FIFO_NOT_EMPTY; //irq on
					while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))u32 src = REG_IPC_FIFO_RX; //get sync irqs back
					return; //and return
				}
		}
		if (pressed&KEY_DOWN && ausgewauhlt != 2){ ausgewauhlt++;}
		if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--;}

	}


}

/*void arm7debugMsgHandler(int bytes, void *user_data)
{
	//ndsMode();
	u8 msg[255]; //max 255
	iprintf("enter");
	fifoGetDatamsg(FIFO_USER_02, bytes, msg);
	iprintf((char*)msg);
	iprintf("exit");
	//if(gbamode)gbaMode2();
}*/

//---------------------------------------------------------------------------------
int main(void) {

  biosPath[0] = 0;
  savePath[0] = 0;





//---------------------------------------------------------------------------------
	//set the mode for 2 text layers and two extended background layers
	//videoSetMode(MODE_5_2D); 

	//defaultExceptionHandler();	//for debug befor gbainit

	//set the first two banks as background memory and the third as sub background memory
	//D is not used..if you need a bigger background then you will need to map
	//more vram banks consecutivly (VRAM A-D are all 0x20000 bytes in size)
	vramSetPrimaryBanks(	VRAM_A_MAIN_BG_0x06000000/*for gba*/, VRAM_B_MAIN_SPRITE/*for gba sprite*/, 
		VRAM_C_SUB_BG /*for prints to lowern screan*/ , /*VRAM_D_LCD*/ VRAM_D_MAIN_BG_0x06020000 /*for BG emulation*/); //needed for main emulator





	irqSet(IRQ_FIFO_NOT_EMPTY,arm7dmareq); //todo async
	irqEnable(IRQ_FIFO_NOT_EMPTY);

	
//the other start at 0x06880000 - 0x068A3FFF

	//bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	consoleDemoInit();

	REG_POWERCNT &= ~((POWER_3D_CORE | POWER_MATRIX) & 0xFFFF);//powerOff(POWER_3D_CORE | POWER_MATRIX); //3D use power so that is not needed

	//consoleDemoInitsubsc();
	//iprintf("gbaemu4DS for r4i gold (3DS) (r4ids.cn) by ichfly\nBuildday" __DATE__);
	//consoleDemoInit();
	//soundEnable(); //sound finaly
	//fifoSetDatamsgHandler(FIFO_USER_02, arm7debugMsgHandler, 0);




	DISPCNT  = 0x0080;

//rootenabelde[2] = fatMountSimple  ("sd", &__io_dsisd); //DSi//sems to be inited by fatInitDefault
 //fatInitDefault();
 //nitroFSInit();

if(!(_io_dldi_stub.friendlyName[0] == 0x52 && _io_dldi_stub.friendlyName[5] == 0x4E))
{
		iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
		iprintf("Warning: you try to run gbaemu DS on %s gbaemu may not work\n press A to continue and ignore this",_io_dldi_stub.friendlyName);
	while(1) {
		scanKeys();
		if (keysDown()&KEY_A) break;
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
	}
}
else
{	
	//antysoftreset
	/*if(*(u32*)((u8*)(IntrMain) + 0x18) == 0xE51FF004)
	{
		int oldIME = enterCriticalSection();
		*(u32*)((u8*)(IntrMain) + 0x18) = 0xE14F0000;
		*(u32*)((u8*)(IntrMain) + 0x1C) = 0xE92D5002;
		leaveCriticalSection(oldIME);


	  //send seed patch
	  //fifoSendValue32(FIFO_USER_01,0x40000000);
	  //fifoSendValue32(FIFO_USER_01,0xAF8CA2B8);


	}*/
}
//data protbuff
#ifdef arm9advsound
REG_IPC_FIFO_TX = 0x4; //load buffer
REG_IPC_FIFO_TX = (u32)malloc(0x100); //buffer for arm7
#endif
//test

/*REG_IPC_FIFO_TX = 0;
while(true)
{
	int i = REG_IPC_FIFO_RX;
	iprintf("%08X\r\n",i);
	REG_IPC_FIFO_TX = i;

}*/

/*	iprintf("\n%x %x %x",getHeapStart(),getHeapEnd(),getHeapLimit());
malloc(0x4000);
iprintf("\n%x %x %x",getHeapStart(),getHeapEnd(),getHeapLimit());
	while(1);*/ //test getHeapEnd() is the needed thing

	iprintf("Init Fat...");
    



    if(fatInitDefault()){
        iprintf("OK\n");
    }else{
        iprintf("failed\n");
        int i = 0;
		while(i< 300)
		{
			//swiWaitForVBlank();
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			i++;
		}
    }

	//irqInit();
	

//main menü ende aber bleibe im while

	

	//dirfolder("nitro:/");
	
	
	bool nichtausgewauhlt = true;
	

	
	iprintf("\x1b[2J");
//main menü

//while(1)iprintf("o");


#ifndef loaddirect

	browseForFile("");
#endif
	
	int ausgewauhlt = 0;


	int myflashsize = 0x10000;
	u32 pressed = 0;


		while(1)
		{
			iprintf("\x1b[2J");

			for(int i = 0; i < 7; i++)
			{
				if(i == ausgewauhlt) iprintf("->");
				else iprintf("  ");
				iprintf(savetypeschar[i]);
				iprintf("\n");
			}
			do 
			{
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));
				scanKeys();
				pressed = keysDownRepeat();
			} while (!pressed);

			if (pressed&KEY_A)
			{
				if(ausgewauhlt == 6)
				{
					myflashsize = 0x20000;
					cpuSaveType = 3;
				}
				else
				{
					cpuSaveType = ausgewauhlt;
				}
				break;
			}
			if (pressed&KEY_DOWN && ausgewauhlt != 6){ ausgewauhlt++;}
			if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--;}
		}







	//sprintf(szFile,"%s","nitro:/puzzle.gba"); //ichfly test
	
	/*scanKeys();

	while(keysDownRepeat()&KEY_A)
	{
		scanKeys();
		swiWaitForVBlank();
	}*/

	bool extraram =false; 
	//if(!REG_DSIMODE) extraram = ram_init(DETECT_RAM); 
	//extraram = true; //testtest
	while(1) 
	{
		iprintf("\x1b[2J");
		iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
		iprintf("fps 60/%i\n",frameskip + 1);
		//swiWaitForVBlank();
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A) break;
		if (isdaas&KEY_UP) frameskip++;
		if (isdaas&KEY_DOWN && frameskip != 0) frameskip--;
	}
	//iprintf("\x1b[2J");

	int syncline =159;
	while(1) 
	{
		iprintf("\x1b[2J");
		iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
		iprintf("Videosyncline %i\n",syncline);
		//swiWaitForVBlank();
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A) break;
		if (isdaas&KEY_UP) syncline++;
		if (isdaas&KEY_DOWN && syncline != 0) syncline--;
	}



  iprintf("\x1b[2J");

  parseDebug = true;

  //if(argc == 2) {
  //iprintf("%s",szFile);
  //while(1);
    bool failed = false;
 
	iprintf("CPULoadRom...");

      failed = !CPULoadRom(szFile,extraram);
	  
	  if(failed)
	  {
		printf("failed");
		while(1);
	  }
	  iprintf("OK\n");
	  	//iprintf("Hello World2!");

		iprintf("CPUInit\n");
		CPUInit(biosPath, useBios,extraram);
	  
	  	

	  iprintf("CPUReset\n");
      CPUReset();
		  

	  if(savePath[0] != 0)
	  {
		  iprintf("CPUReadBatteryFile...");
		  if(CPUReadBatteryFile(savePath))
		  {
			iprintf("OK\n");
		  }
		  else
		  {
			  iprintf("failed\n");
			  while(1);
		  }
	  }
	if(cpuSaveType == 3)flashSetSize(myflashsize);
	
	gbaHeader_t *gbaGame;

	
	gbaGame = (gbaHeader_t*)rom;

	iprintf("BIOS_RegisterRamReset\n");

	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu to write to the internalRAM

	BIOS_RegisterRamReset(0xFF);

	pu_Enable();
	
	
	//memcopy((void*)0x2000000,(void*)rom, 0x40000);
	
	iprintf("dmaCopy\n");

/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x7654321;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/

	dmaCopy( (void*)rom,(void*)0x2000000, 0x40000);
	
	iprintf("arm7init\n");


	REG_IPC_FIFO_TX = 0x1FFFFFFF; //cmd
	REG_IPC_FIFO_TX = syncline;
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))u32 src = REG_IPC_FIFO_RX;

	iprintf("irqinit\n");

	anytimejmpfilter = 0;
	
	//anytimejmp = (VoidFn)0x3007FFC; //bios import
/*
REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x1234567;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
	iprintf("emulateedbiosstart\n");

	emulateedbiosstart();
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x1111111;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
    iprintf("ndsMode\n");

	ndsMode();
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x2222222;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
    iprintf("gbaInit\n");

	gbaInit();
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x3333333;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
	iprintf("irqSet\n");

	//irqSet(IRQ_VBLANK, VblankHandler);

	//irqEnable(IRQ_VBLANK);
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x4444444;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
	iprintf("gbaMode2\n");

gbamode = true;
	
	gbaMode2();


	iprintf("jump to (%08X)\n\r",rom);

	//iprintf("\x1b[2J"); //reset (not working huh)

	cpu_ArmJumpforstackinit((u32)rom, 0);
	
	
	
  
  while(true);

		 
  return 0;

}
//a
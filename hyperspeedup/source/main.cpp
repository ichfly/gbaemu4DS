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



//copy protection

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
struct EmulatedSystem emulator;

char* rootdirnames [3] = {"nitro:/","fat:/","sd:/"};
bool rootenabelde[3];

int ausgewauhlt = 0;

char* currentdir =  (char*)0;

int seite = 0;

int dirfeldsize = 0;
char** names; //stupid i know but i don't know a better way
u32* dirfeldsizes;


int bg = 0;

char* memoryWaitrealram[8] =
  { "10 and 6","8 and 6","6 and 6","18 and 6","10 and 4","8 and 4","6 and 4","18 and 4" };





extern "C" void IntrMain();


extern "C" void testasm(u32* feld);
extern "C" void cpu_SetCP15Cnt(u32 v);
extern "C" u32 cpu_GetCP15Cnt();
extern "C" u32 pu_Enable();

int ignorenextY = 0;

void speedtest()
{	
	/*(volatile u32*)0x4000214 = 0x8;
	iprintf("\x1b[2J");
	iprintf("fps %d",framenummer);
	framenummer = 0;*/
}





bool dirfolder (char* folder)
{	
		DIR *pdir;
		struct dirent *pent;
		struct stat statbuf;

		pdir=opendir(folder);

		if (pdir){
			
			while ((pent=readdir(pdir))!=NULL) {
	    		stat(pent->d_name,&statbuf);
	    		if(strcmp(".", pent->d_name) == 0)
	        		continue;
				void* temp = malloc((dirfeldsize + 1)*4);
				memcpy( (void *)temp , (void *)dirfeldsizes , (dirfeldsize)*4);
				//iprintf ("%x\n", (u32)dirfeldsizes);
				if(dirfeldsize > 2) free(dirfeldsizes); //memory leak
				dirfeldsizes = (u32*)temp;
				char** temp2 = (char**)malloc((dirfeldsize + 1)*4);
				//iprintf ("%x\n", (u32)temp2);
				memcpy( (void *)temp2 , (void *)names , (dirfeldsize)*4);
				if(dirfeldsize > 2)free(names); //memory leak
				names = temp2;
				//iprintf ("%x\n", (u32)temp2);
				//iprintf ("%x\n", (u32)names);
	    		if(S_ISDIR(statbuf.st_mode))
				{
	        		*(u32*)(dirfeldsize + dirfeldsizes) = 0xFFFFFFFF;
					//iprintf("%s <dir>%d %d\n ", pent->d_name,dirfeldsize,*(u32*)(dirfeldsize + dirfeldsizes));
				}
	    		if(!(S_ISDIR(statbuf.st_mode)))
				{
					*(u32*)(dirfeldsize + dirfeldsizes) = statbuf.st_size;
					//iprintf ("%d\n", *(u32*)(dirfeldsize + dirfeldsizes));
					//iprintf("%s %ld\n", pent->d_name, statbuf.st_size);
	        	}
				*(char**)((u32)names + dirfeldsize * 4) = (char*)malloc(strlen(pent->d_name) + 2);
				sprintf(*(char**)((u32)names + dirfeldsize * 4),"%s",pent->d_name);
				dirfeldsize++;
				//while(1);
				//iprintf ("%d\n", dirfeldsize);
				//free(temp2);
				//free(temp);
			}
			closedir(pdir);
		} else {
			//iprintf ("opendir() failure; terminating\n");
			return false;
		}
	/*for(int i = 0; i < dirfeldsize; i++)
	{
		//iprintf ("%d\n", *(u32*)((u32)dirfeldsizes + i * 4));
		iprintf ("%x\n", (char**)((u32)names + i * 4));
	}
	iprintf ("%d\n",dirfeldsize);*/
	return true;

}



void dirfree ()
{
	for(int i = 0; i < dirfeldsize - 1; i++)
	{
		iprintf ("%x\n", *(u32*)((u32)names + i * 4));
		free(*(char**)((u32)names + i * 4));
	}
	//while(1);
	free(names);
	if(dirfeldsize > 2)free(dirfeldsizes); //malloc return 0x341 when i free that but why
	dirfeldsize = 0;
}



void display ()
{
	for(int i = ausgewauhlt - (ausgewauhlt%20); i < dirfeldsize && i < ausgewauhlt - (ausgewauhlt%20) + 20 ; i++)
	{
		if(i == ausgewauhlt)iprintf("->");
		else iprintf("  ");

		if(*(u32*)(i + dirfeldsizes) == 0xFFFFFFFF)iprintf("%s <dir>\n", *(char**)((u32)names + i * 4));
		else iprintf("%s (%ld)\n", *(char**)((u32)names + i * 4), *(u32*)(dirfeldsizes + i));
	}
}

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






int bgrouid;





int frameskip = 10;

int framewtf = 0;


int oldmode = 0;
u16 lastDISPCNT = 0;


extern "C" int SPtoload;
extern "C" int SPtemp;

//---------------------------------------------------------------------------------
void HblankHandler(void) {
//---------------------------------------------------------------------------------
	CPUCheckDMA(2, 0x0f);
	/*if(IME & 1 && IE & 2)
	{
		IE &= 2;
		gbaMode();
		cpu_ArmJump(*(u32*)0x3007FFC, 0);
		ndsMode();
	}*/
	//*(volatile u32*)0x4000214 = 0x2;
	//iprintf("test");
}
//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	
		//*(volatile u32*)0x4000214 = 0x1;
		//while(1);
	
	  /*fifoSendValue32(FIFO_USER_01,0x80000000); //request data
	  fifoSendValue32(FIFO_USER_01,0x80000000); //dummy*/


	//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
#ifdef arm9advsound
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //inter sounddma
	{
		arm7dmareq();
	}
#endif
	CPUCheckDMA(1, 0x0f);
	
	//Log("VB %X %X ",SPtoload,SPtemp);
	//iprintf("i%x %x %x %x %x %x\r\n",IE,IF_VBl,anytimejmpfilter,REG_IE,REG_IF,IME);


	//iprintf("DISPCNT2fly %x %x\r\n",DISPCNT,workaroundread16((u16*)&DISPCNT));
	//iprintf("%x",workaroundread16((u16*)&DISPCNT));	
	
	if(framewtf == frameskip)
	{
	//iprintf("%x %x %x %x %x %x\r\n",IE,IF_VBl,anytimejmpfilter,REG_IE,REG_IF,IME);
		//REG_IPC_FIFO_TX = 0x80000000;
		//REG_IPC_FIFO_TX = 0x5B468E37;
	  //iprintf("enter");
		/*u8 msg[255]; //max 255
		//iprintf("enter");
		fifoGetDatamsg(FIFO_USER_02, 100, msg);
		iprintf((char*)msg);*/
		//iprintf("exit");
		while(dmaBusy(3)); // ichfly wait for dma 3
//#ifndef public
		//iprintf("DISPCNT %x %x\r\n",DISPCNT,REG_DISPCNT);
//#endif


		framewtf = 0;
		//iprintf("DISPCNT2fly %x %x\r\n",&DISPCNT,workaroundread16((u16*)&DISPCNT));
		/*for(int iy = 0; iy <0x200 ; iy++)
		{
			*(u16 *)(0x07000000 + 2*iy) = *(u16 *)(oam + 2*iy);
		}
			for(int iy = 0; iy <0x200 ; iy++)
		{
			*(u16 *)(0x05000000 + 2*iy) = *(u16 *)(paletteRAM + 2*iy);
			
		}*/
		
		if((DISPCNT & 7) < 3)
		{
			dmaCopyWordsAsynch(1,(void*)vram + 0x10000,(void*)0x06400000,0x8000);
			if(lastDISPCNT != DISPCNT)
			{
				//reset BG3HOFS and BG3VOFS
				REG_BG3HOFS = BG3HOFS;
				REG_BG3VOFS = BG3VOFS;

				//reset
				REG_BG3CNT = BG3CNT;
				REG_BG2CNT = BG2CNT;
				REG_BLDCNT = BLDMOD;
				WIN_IN = WININ;
				WIN_OUT = WINOUT;

				REG_BG2PA = BG2PA;
				REG_BG2PB = BG2PB;
				REG_BG2PC = BG2PC;
				REG_BG2PD = BG2PD;
				REG_BG2X = (BG2X_L | (BG2X_H << 16));
				REG_BG2Y = (BG2Y_L | (BG2Y_H << 16));

				REG_BG3PA = BG3PA;
				REG_BG3PB = BG3PB;
				REG_BG3PC = BG3PC;
				REG_BG3PD = BG3PD;
				REG_BG3X = (BG3X_L | (BG3X_H << 16));
				REG_BG3Y = (BG3Y_L | (BG3Y_H << 16));

				u32 dsValue;
				dsValue  = DISPCNT & 0xFF87;
				dsValue |= (DISPCNT & (1 << 5)) ? (1 << 23) : 0;	/* oam hblank access */
				dsValue |= (DISPCNT & (1 << 6)) ? (1 << 4) : 0;	/* obj mapping 1d/2d */
				dsValue |= (DISPCNT & (1 << 7)) ? 0 : (1 << 16);	/* forced blank => no display mode (both)*/
				REG_DISPCNT = dsValue; 
			}
			//iprintf("%08x %08x %08x %08x %08x\n",workaroundread16((u16*)&DISPCNT),*(u32*)(0x05000204),*(u32*)(0x07000004),workaroundread32((u32*)&REG_DISPCNT)/*REG_DISPCNT*/,*(u32*)(0x6014020));
			//dmaCopyWordsAsynch(0,vram,(void*)0x06000000,0x10000);
			lastDISPCNT = DISPCNT;
		}
		else
		{
#ifndef public
			//iprintf("%x\r\n",*(u32*)(0x0640403C));
#endif
			if(lastDISPCNT != DISPCNT)
			{
				//reset BG3HOFS and BG3VOFS
				REG_BG3HOFS = 0;
				REG_BG3VOFS = 0;

				//BLDCNT(2 enabeled bits)
				int tempBLDMOD = BLDMOD & ~0x404;
				tempBLDMOD = tempBLDMOD | ((BLDMOD & 0x404) << 1);
				REG_BLDCNT = tempBLDMOD;

				//WINOUT(2 enabeled bits)
				int tempWINOUT = WINOUT & ~0x404;
				tempWINOUT = tempWINOUT | ((WINOUT & 0x404) << 1);
				WIN_OUT = tempWINOUT;

				//WININ(2 enabeled bits)
				int tempWININ = WININ & ~0x404;
				tempWININ = tempWININ | ((WININ & 0x404) << 1);
				WIN_IN = tempWININ;

				//swap LCD I/O BG Rotation/Scaling

				REG_BG3PA = BG2PA;
				REG_BG3PB = BG2PB;
				REG_BG3PC = BG2PC;
				REG_BG3PD = BG2PD;
				REG_BG3X = (BG2X_L | (BG2X_H << 16));
				REG_BG3Y = (BG2Y_L | (BG2Y_H << 16));


				u32 dsValue;
				dsValue  = DISPCNT & 0xF087;
				dsValue |= (DISPCNT & (1 << 5)) ? (1 << 23) : 0;	/* oam hblank access */
				dsValue |= (DISPCNT & (1 << 6)) ? (1 << 4) : 0;	/* obj mapping 1d/2d */
				dsValue |= (DISPCNT & (1 << 7)) ? 0 : (1 << 16);	/* forced blank => no display mode (both)*/
				REG_DISPCNT = dsValue; //workaroundwrite32(dsValue, (u32*)&REG_DISPCNT);
                                       //REG_DISPCNT = (workaroundread16((u16*)&DISPCNT) | 0x02010010) & ~0x400; //need 0x10010
				if((DISPCNT & 7) == 4)
				
				{
					bgrouid = bgInit_call(3, BgType_Bmp8, BgSize_B8_256x256,8,8); //(3, BgType_Bmp16, BgSize_B16_256x256, 0,0); //sassert(tileBase == 0 || type < BgType_Bmp8, "Tile base is unused for bitmaps.  Can be offset using mapBase * 16KB"); kind of not needed
				}
				else if((DISPCNT & 7) == 3)bgrouid = bgInit_call(3, BgType_Bmp16, BgSize_B16_256x256,8,8);
				else if((DISPCNT & 7) == 5)bgrouid = bgInit_call(3, BgType_Bmp16, BgSize_B16_256x256,8,8);
				REG_BG3CNT = REG_BG3CNT | (BG2CNT & 0x43); //swap BG2CNT (BG Priority and Mosaic) 
			}
			if((DISPCNT & 7) == 3) //BG Mode 3 - 240x160 pixels, 32768 colors
			{
				u8 *pointertobild = (u8 *)(0x6000000);
				for(int iy = 0; iy <160; iy++){
					dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+512*(iy), 480);
					pointertobild+=480;
				}
			}	
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
			if((DISPCNT & 7) == 5) //BG Mode 5 - 160x128 pixels, 32768 colors
			{
				u8 *pointertobild = (u8 *)(0x6000000);
				if(BIT(4) & DISPCNT)pointertobild+=0xA000;
				for(int iy = 0; iy <128; iy++){
					dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+512*(iy), 320);
					pointertobild+=320;
				}
			}
			//iprintf("b");
			//iprintf("DISPCNT5fly %x\r\n",workaroundread16((u16*)&DISPCNT));
			
			//iprintf("DISPCNT2.5 %x\r\n",workaroundread16((u16*)&DISPCNT));
			
			//dmaCopyWordsAsynch(0,vram,(void*)0x06000000,0x14000);
			dmaCopyWordsAsynch(1,(void*)0x06014000,(void*)0x06404000,0x4000);
			//dmaCopy(vram, bgGetGfxPtr(bgrouid), 240*160);
			lastDISPCNT = DISPCNT;
		}
		//iprintf("c");
		/*if(DISPCNT != 0)
		{
			iprintf("DISPCNT3 %x\r\n",workaroundread16((u16*)&DISPCNT));
		}*/
	}
	else
	{
		framewtf++;
	}
	

	
		//scanKeys();
  
  u32 joy = ((~REG_KEYINPUT)&0x3ff);
#ifdef ichflytestkeypossibillity  
  
  // disallow L+R or U+D of being pressed at the same time
  if((joy & 48) == 48)
    joy &= ~16;
  if((joy & 192) == 192)
    joy &= ~128;
#endif
	if((joy & KEY_A) && (joy & KEY_B) && (joy & KEY_R) && (joy & KEY_L))
	{
		if(ignorenextY == 0)
		{
			pausemenue();
			ignorenextY = 60; // 1 sec break time
		}
		else {ignorenextY -= 1;}
	}
    P1 = 0x03FF ^ joy;             
    UPDATE_REG(0x130, P1);

	//cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu to write to the internalRAM


	/*while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
	{
		u32 temprx = REG_IPC_FIFO_RX;
		iprintf("%08X %08X\r\n",temprx,*(u32*)temprx);
	}*/

	//pu_Enable();
	
	if(IE & 1)IF_VBl = 1;

	
	//iprintf("test"); //sorry no write here
	//*(u32*)0x2003000 = *(u32*)0x2003000 + 1;
	/*if(IME & 1 && IE & 1)
	{
		IE &= 1;
		gbaMode();
		cpu_ArmJump(*(u32*)0x3007FFC, 0);
		ndsMode();
	}*/
	
	//iprintf("%x %x %x %x %x %x %x %x %x %x \r\n",DISPCNT,BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,BG2PA, BG2PB, BG2PC, BG2PD);
}







void frameasyncsync(void) {
//---------------------------------------------------------------------------------
	
		while(dmaBusy(3)); // ichfly wait for dma 3
		framewtf = 0;
		if((DISPCNT & 7) < 3)
		{
			dmaCopyWordsAsynch(1,(void*)vram + 0x10000,(void*)0x06400000,0x8000);
			if(lastDISPCNT != DISPCNT)
			{
				//reset BG3HOFS and BG3VOFS
				REG_BG3HOFS = BG3HOFS;
				REG_BG3VOFS = BG3VOFS;

				//reset
				REG_BG3CNT = BG3CNT;
				REG_BG2CNT = BG2CNT;
				REG_BLDCNT = BLDMOD;
				WIN_IN = WININ;
				WIN_OUT = WINOUT;

				REG_BG2PA = BG2PA;
				REG_BG2PB = BG2PB;
				REG_BG2PC = BG2PC;
				REG_BG2PD = BG2PD;
				REG_BG2X = (BG2X_L | (BG2X_H << 16));
				REG_BG2Y = (BG2Y_L | (BG2Y_H << 16));

				REG_BG3PA = BG3PA;
				REG_BG3PB = BG3PB;
				REG_BG3PC = BG3PC;
				REG_BG3PD = BG3PD;
				REG_BG3X = (BG3X_L | (BG3X_H << 16));
				REG_BG3Y = (BG3Y_L | (BG3Y_H << 16));

				u32 dsValue;
				dsValue  = DISPCNT & 0xFF87;
				dsValue |= (DISPCNT & (1 << 5)) ? (1 << 23) : 0;	/* oam hblank access */
				dsValue |= (DISPCNT & (1 << 6)) ? (1 << 4) : 0;	/* obj mapping 1d/2d */
				dsValue |= (DISPCNT & (1 << 7)) ? 0 : (1 << 16);	/* forced blank => no display mode (both)*/
				REG_DISPCNT = dsValue; 
			}
			lastDISPCNT = DISPCNT;
		}
		else
		{
			if(lastDISPCNT != DISPCNT)
			{
				//reset BG3HOFS and BG3VOFS
				REG_BG3HOFS = 0;
				REG_BG3VOFS = 0;

				//BLDCNT(2 enabeled bits)
				int tempBLDMOD = BLDMOD & ~0x404;
				tempBLDMOD = tempBLDMOD | ((BLDMOD & 0x404) << 1);
				REG_BLDCNT = tempBLDMOD;

				//WINOUT(2 enabeled bits)
				int tempWINOUT = WINOUT & ~0x404;
				tempWINOUT = tempWINOUT | ((WINOUT & 0x404) << 1);
				WIN_OUT = tempWINOUT;

				//WININ(2 enabeled bits)
				int tempWININ = WININ & ~0x404;
				tempWININ = tempWININ | ((WININ & 0x404) << 1);
				WIN_IN = tempWININ;

				//swap LCD I/O BG Rotation/Scaling

				REG_BG3PA = BG2PA;
				REG_BG3PB = BG2PB;
				REG_BG3PC = BG2PC;
				REG_BG3PD = BG2PD;
				REG_BG3X = (BG2X_L | (BG2X_H << 16));
				REG_BG3Y = (BG2Y_L | (BG2Y_H << 16));


				u32 dsValue;
				dsValue  = DISPCNT & 0xF087;
				dsValue |= (DISPCNT & (1 << 5)) ? (1 << 23) : 0;	/* oam hblank access */
				dsValue |= (DISPCNT & (1 << 6)) ? (1 << 4) : 0;	/* obj mapping 1d/2d */
				dsValue |= (DISPCNT & (1 << 7)) ? 0 : (1 << 16);	/* forced blank => no display mode (both)*/
				REG_DISPCNT = dsValue; //workaroundwrite32(dsValue, (u32*)&REG_DISPCNT);
                                       //REG_DISPCNT = (workaroundread16((u16*)&DISPCNT) | 0x02010010) & ~0x400; //need 0x10010
				if((DISPCNT & 7) == 4)
				
				{
					bgrouid = bgInit_call(3, BgType_Bmp8, BgSize_B8_256x256,8,8); //(3, BgType_Bmp16, BgSize_B16_256x256, 0,0); //sassert(tileBase == 0 || type < BgType_Bmp8, "Tile base is unused for bitmaps.  Can be offset using mapBase * 16KB"); kind of not needed
				}
				else if((DISPCNT & 7) == 3)bgrouid = bgInit_call(3, BgType_Bmp16, BgSize_B16_256x256,8,8);
				else if((DISPCNT & 7) == 5)bgrouid = bgInit_call(3, BgType_Bmp16, BgSize_B16_256x256,8,8);
				REG_BG3CNT = REG_BG3CNT | (BG2CNT & 0x43); //swap BG2CNT (BG Priority and Mosaic) 
			}
			if((DISPCNT & 7) == 3) //BG Mode 3 - 240x160 pixels, 32768 colors
			{
				u8 *pointertobild = (u8 *)(0x6000000);
				for(int iy = 0; iy <160; iy++){
					dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+512*(iy), 480);
					pointertobild+=480;
				}
			}	
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
			if((DISPCNT & 7) == 5) //BG Mode 5 - 160x128 pixels, 32768 colors
			{
				u8 *pointertobild = (u8 *)(0x6000000);
				if(BIT(4) & DISPCNT)pointertobild+=0xA000;
				for(int iy = 0; iy <128; iy++){
					dmaCopy( (void*)pointertobild, (void*)0x6020000/*bgGetGfxPtr(bgrouid)*/+512*(iy), 320);
					pointertobild+=320;
				}
			}
			dmaCopyWordsAsynch(1,(void*)0x06014000,(void*)0x06404000,0x4000);
			lastDISPCNT = DISPCNT;
		}
	}







bool gbamode = false;



char* seloptions [3] = {"save save","show mem","Continue (Beta)"};

void pausemenue()
{
	irqDisable(IRQ_VBLANK);
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
					if(savePath[0] == 0)sprintf(savePath,"%s.save.bin",szFile);
					CPUWriteBatteryFile(savePath);
					break;
				case 1:
					show_mem();
					break;
				case 2:
					cpupausemodeexit();
					iprintf("\x1b[2J");
					while(REG_VCOUNT != 192); //wait for Vblanc
					irqEnable( IRQ_VBLANK);
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



#ifdef arm9advsound
	irqSet(IRQ_FIFO_NOT_EMPTY,arm7dmareq);
	irqEnable(IRQ_FIFO_NOT_EMPTY);
#endif

	
//the other start at 0x06880000 - 0x068A3FFF

	//bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	consoleDemoInit();

	powerOff(POWER_3D_CORE | POWER_MATRIX); //3D use power so that is not needed

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
	if(*(u32*)((u8*)(IntrMain) + 0x18) == 0xE51FF004)
	{
		int oldIME = enterCriticalSection();
		*(u32*)((u8*)(IntrMain) + 0x18) = 0xE14F0000;
		*(u32*)((u8*)(IntrMain) + 0x1C) = 0xE92D5002;
		leaveCriticalSection(oldIME);


	  //send seed patch
	  //fifoSendValue32(FIFO_USER_01,0x40000000);
	  //fifoSendValue32(FIFO_USER_01,0xAF8CA2B8);


	}
}
//data protbuff
REG_IPC_FIFO_TX = 0x4; //load buffer
REG_IPC_FIFO_TX = (u32)malloc(0x100); //buffer for arm7
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
	/*while(nichtausgewauhlt)
	{
		for(int i = 0; i < 3; i++)
		{
			if(i == ausgewauhlt) printf("->");
			else printf("  ");
			printf(rootdirnames[i]);
			printf("\n");
		}
		while(nichtausgewauhlt)
		{
			swiWaitForVBlank();
			scanKeys();
			if (keysDown()&KEY_A)
			{
				if(ausgewauhlt == 0)
				{
					iprintf("Init Nitro...\n");
					if(nitroFSInit())
					iprintf("Nitro OK.\n");
					else{
						iprintf("Nitro Fail.\n");
						int i = 0;
						while(i< 300)
						{
							swiWaitForVBlank();
							i++;
						}
						
					}
					swiWaitForVBlank();
				}
				
				int ausgewauhlt2 = ausgewauhlt;
				dirfolder(rootdirnames[ausgewauhlt2]);
				//menue start
				
				while(nichtausgewauhlt)
				{
					iprintf("\x1b[2J");
					display();
					while(nichtausgewauhlt)
					{
						swiWaitForVBlank();
						scanKeys();
						if (keysDown()&KEY_DOWN){ ausgewauhlt++; if(ausgewauhlt == dirfeldsize)ausgewauhlt = 0;  break;}
						if (keysDown()&KEY_UP && ausgewauhlt != 0) { if(ausgewauhlt == 0)ausgewauhlt = dirfeldsize;else {ausgewauhlt--;} break;}
						if (keysDown()&KEY_A)
						{
							if(currentdir == (char*)0)
							{
								currentdir = (char*)malloc(strlen(rootdirnames[ausgewauhlt2]) + strlen(*(char**)((u32)names + ausgewauhlt * 4)) + 4);
								sprintf(currentdir,"%s%s",rootdirnames[ausgewauhlt2],*(char**)((u32)names + ausgewauhlt * 4));
							}
							else
							{
								char* currentdirtemp = (char*)malloc(strlen(currentdir) + strlen(*(char**)((u32)names + ausgewauhlt * 4)) + 4);
								sprintf(currentdirtemp,"%s/%s",currentdir,*(char**)((u32)names + ausgewauhlt * 4));								
								//free(currentdir);
								currentdir = currentdirtemp;
							}
							if(!(*(u32*)(dirfeldsizes + ausgewauhlt) == 0xFFFFFFFF))
							{
								sprintf(szFile,"%s",currentdir);
								free(currentdir);
								nichtausgewauhlt = false;
							}
							//iprintf(currentdir);
							dirfree();
							dirfolder(currentdir);
							ausgewauhlt = 0;
							break;
						}
					}
				}
				//menue ende
				//break;
			}
			if (keysDown()&KEY_DOWN && ausgewauhlt != 2){ ausgewauhlt++; break;}
			if (keysDown()&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--; break;}
		}
		iprintf("\x1b[2J");	
	}
	dirfree();*/
#endif
	
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
       emulator = GBASystem;

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
	
	iprintf("irqinit\n");

	anytimejmpfilter = 0;
	
	anytimejmp = (VoidFn)0x3007FFC;
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

	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable(IRQ_VBLANK);
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x4444444;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
	iprintf("gbaMode2\n");

gbamode = true;
	
	gbaMode2();


	iprintf("jump to (%08X)\n\r",rom);

	//iprintf("\x1b[2J"); //reset

	cpu_ArmJumpforstackinit((u32)rom, 0);
	
	
	
  
  while(true);

		 
  return 0;

}
//a
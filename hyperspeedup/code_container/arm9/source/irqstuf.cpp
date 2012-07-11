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



extern "C" void resettostartup();

extern "C" void IntrMain();


extern "C" void testasm(u32* feld);
extern "C" void cpu_SetCP15Cnt(u32 v);
extern "C" u32 cpu_GetCP15Cnt();
extern "C" u32 pu_Enable();


int main( int argc, char **argv);


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

#ifdef usebuffedVcout
u8 VCountgbatods[0x100]; //(LY)      (0..227) + check overflow
u8 VCountdstogba[263]; //(LY)      (0..262)
u8 VCountdoit[263]; //jump in or out
#endif

extern "C" int SPtoload;
extern "C" int SPtemp;

#ifdef usebuffedVcout

void initspeedupfelder()
{
	/*bool modusVcount = true;
	while(1) 
	{
		iprintf("\x1b[2J");
		iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
		if(modusVcount)iprintf("speedimpr. Vcount mode\n");
		else iprintf("dispimpr. Vcount mode\n");
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A) break;
		if (isdaas&KEY_UP) modusVcount = true;
		if (isdaas&KEY_DOWN) modusVcount = false;
	}


	if(modusVcount)*/
	{
		s16 currentprocV = 0;
		s16 res1 = 0;
		s16 resold = -1;
		s16 currentVCountgbatods = 0;
		while(currentprocV < 263)
		{
			if(currentprocV < 192)
			{
				res1 = ((currentprocV * 214) >> 8);//VCOUNT = help * (1./1.2); //1.15350877;
				//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
			}
			else
			{
				res1 = (((currentprocV - 192) * 246) >>  8)+ 160;//VCOUNT = ((temp - 192) * (1./ 1.04411764)) + 160 //* (1./ 1.04411764)) + 160; //1.15350877;
				//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
			}
			VCountdstogba[currentprocV] = res1;
			VCountdoit[currentprocV] = (resold == res1) ? false : true;
			if(resold != res1)
			{
				if(currentprocV > 0xFF)
				{
					VCountgbatods[currentVCountgbatods] = 0xFF;
				}
				else
				{
					VCountgbatods[currentVCountgbatods] = currentprocV;
				}
			}
			resold = res1;
			currentprocV++;
		}
	}
	/*else 
	{
		s16 currentprocV = 0;
		s16 res1 = 0;
		s16 resold = -1;
		s16 currentVCountgbatods = 0;
		while(currentprocV < 263)
		{
			if(currentprocV < 192)
			{
				res1 = currentprocV;
				if(res1 > 159) res1 = 159;
			}
			else
			{
				res1 = (((currentprocV - 192) * 246) >>  8)+ 160;//VCOUNT = ((temp - 192) * (1./ 1.04411764)) + 160 //* (1./ 1.04411764)) + 160; //1.15350877;
				//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
			}
			VCountdstogba[currentprocV] = res1;
			VCountdoit[currentprocV] = (resold == res1) ? false : true;
			if(resold != res1)
			{
				if(currentprocV > 0xFF)
				{
					VCountgbatods[currentVCountgbatods] = 0xFF;
				}
				else
				{
					VCountgbatods[currentVCountgbatods] = currentprocV;
				}
			}
			resold = res1;
			currentprocV++;
		}
	}*/

}
#endif
//---------------------------------------------------------------------------------
void HblankHandler(void) {
//---------------------------------------------------------------------------------

#ifdef usebuffedVcout

	if(VCountdoit[REG_VCOUNT])
	{
		REG_IF = IRQ_HBLANK;
#ifdef HBlankdma
		CPUCheckDMA(2, 0x0f);
#endif
	}
#ifdef forceHBlankirqs
	if(!(IE & IRQ_HBLANK))REG_IF = IRQ_HBLANK;
#endif


#else
	u16 res1;
	u16 res2;
	u16 temp = REG_VCOUNT;
	if(temp < 192)
	{
		res1 = ((temp * 214) >> 8);//VCOUNT = help * (1./1.2); //1.15350877;
		//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
	}
	else
	{
		res1 = (((temp - 192) * 246) >>  8)+ 160;//VCOUNT = ((temp - 192) * (1./ 1.04411764)) + 160 //* (1./ 1.04411764)) + 160; //1.15350877;
		//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
	}
	temp++;
	if(temp < 192)
	{
		res2 = ((temp * 214) >> 8);//VCOUNT = help * (1./1.2); //1.15350877;
		//help3 = (help + 1) * (1./1.2); //1.15350877;  // ichfly todo it is to slow
	}
	else
	{
		res2 = (((temp - 192) * 246) >>  8)+ 160;//VCOUNT = ((temp - 192) * (1./ 1.04411764)) + 160 //* (1./ 1.04411764)) + 160; //1.15350877;
		//help3 = ((help - 191) *  (1./ 1.04411764)) + 160; //1.15350877;  // ichfly todo it is to slow			
	}


	if(res1 == res2)
	{
		REG_IF = IRQ_HBLANK;
#ifdef HBlankdma
		CPUCheckDMA(2, 0x0f);
#endif
	}
#ifdef forceHBlankirqs
	if(!(IE & IRQ_HBLANK))REG_IF = IRQ_HBLANK;
#endif
#endif
}
//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif

#ifdef lastdebug
lasttime[lastdebugcurrent] = 0x0000000;
lastdebugcurrent++;
if(lastdebugcurrent == lastdebugsize)lastdebugcurrent = 0;
#endif

	CPUCheckDMA(1, 0x0f); //V-Blank

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








char* seloptions [4] = {"save save","show mem","Continue","load GBA"};

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
		for(int i = 0; i < 4; i++)
		{
			if(i == ausgewauhlt) iprintf("->");
			else iprintf("  ");
			iprintf(seloptions[i]);
			iprintf("\n");
		}
		do {
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			scanKeys();
			pressed = (keysDownRepeat()& ~0xFC00);
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
				case 3:
					resettostartup();
					//main(0,0);
				}
		}
		if (pressed&KEY_DOWN && ausgewauhlt != 3){ ausgewauhlt++;}
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

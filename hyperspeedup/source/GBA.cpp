//ichfly test


#define loadindirect


//#define DEV_VERSION


// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2005-2006 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <nds.h>
#include <stdio.h>

#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"


#ifndef loadindirect
#include "puzzleorginal_bin.h"
#endif


extern int framenummer;


#define ichflytestkeypossibillity

bool ichflytest = false;

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
//#include "Gfx.h" //ichfly not that
#include "EEprom.h"
#include "Flash.h"
#include "Sound.h"
#include "Sram.h"
#include "bios.h"
#include "Cheats.h"
#include "NLS.h"
#include "elf.h"
#include "Util.h"
#include "Port.h"
#include "agbprint.h"
#ifdef PROFILING
#include "prof/prof.h"
#endif

#define UPDATE_REG(address, value)\
  {\
    WRITE16LE(((u16 *)&ioMem[address]),value);\
  }\

#define ARM_PREFETCH \
  {\
    cpuPrefetch[0] = CPUReadMemoryQuick(armNextPC);\
    cpuPrefetch[1] = CPUReadMemoryQuick(armNextPC+4);\
  }

#define THUMB_PREFETCH \
  {\
    cpuPrefetch[0] = CPUReadHalfWordQuick(armNextPC);\
    cpuPrefetch[1] = CPUReadHalfWordQuick(armNextPC+2);\
  }

#define ARM_PREFETCH_NEXT \
  cpuPrefetch[1] = CPUReadMemoryQuick(armNextPC+4);

#define THUMB_PREFETCH_NEXT\
  cpuPrefetch[1] = CPUReadHalfWordQuick(armNextPC+2);

#ifdef __GNUC__
#define _stricmp strcasecmp
#endif

extern int bg;

extern int emulating;
int SWITicks = 0;
int IRQTicks = 0;

u32 mastercode = 0;
int layerEnableDelay = 0;
bool busPrefetch = false;
bool busPrefetchEnable = false;
u32 busPrefetchCount = 0;
int cpuDmaTicksToUpdate = 0;
int cpuDmaCount = 0;
bool cpuDmaHack = false;
u32 cpuDmaLast = 0;
int dummyAddress = 0;

bool cpuBreakLoop = false;
int cpuNextEvent = 0;

int gbaSaveType = 0; // used to remember the save type on reset
bool intState = false;
bool stopState = false;
bool holdState = false;
int holdType = 0;
bool cpuSramEnabled = true;
bool cpuFlashEnabled = true;
bool cpuEEPROMEnabled = true;
bool cpuEEPROMSensorEnabled = false;

u32 cpuPrefetch[2];

int cpuTotalTicks = 0;
#ifdef PROFILING
int profilingTicks = 0;
int profilingTicksReload = 0;
static profile_segment *profilSegment = NULL;
#endif

/*#ifdef BKPT_SUPPORT //ichfly test
u8 freezeWorkRAM[0x40000];
u8 freezeInternalRAM[0x8000];
u8 freezeVRAM[0x18000];
u8 freezePRAM[0x400];
u8 freezeOAM[0x400];
bool debugger_last;
#endif*/

int lcdTicks = (useBios && !skipBios) ? 1008 : 208;
u8 timerOnOffDelay = 0;
u16 timer0Value = 0;
bool timer0On = false;
int timer0Ticks = 0;
int timer0Reload = 0;
int timer0ClockReload  = 0;
u16 timer1Value = 0;
bool timer1On = false;
int timer1Ticks = 0;
int timer1Reload = 0;
int timer1ClockReload  = 0;
u16 timer2Value = 0;
bool timer2On = false;
int timer2Ticks = 0;
int timer2Reload = 0;
int timer2ClockReload  = 0;
u16 timer3Value = 0;
bool timer3On = false;
int timer3Ticks = 0;
int timer3Reload = 0;
int timer3ClockReload  = 0;
u32 dma0Source = 0;
u32 dma0Dest = 0;
u32 dma1Source = 0;
u32 dma1Dest = 0;
u32 dma2Source = 0;
u32 dma2Dest = 0;
u32 dma3Source = 0;
u32 dma3Dest = 0;
void (*cpuSaveGameFunc)(u32,u8) = flashSaveDecide;
//void (*renderLine)() = mode0RenderLine;
bool fxOn = false;
bool windowOn = false;
int frameCount = 0;
char buffer[1024];
FILE *out = NULL;
u32 lastTime = 0;
int count = 0;

int capture = 0;
int capturePrevious = 0;
int captureNumber = 0;

const int TIMER_TICKS[4] = {
  0,
  6,
  8,
  10
};

const u32  objTilesAddress [3] = {0x010000, 0x014000, 0x014000};
const u8 gamepakRamWaitState[4] = { 4, 3, 2, 8 };
const u8 gamepakWaitState[4] =  { 4, 3, 2, 8 };
const u8 gamepakWaitState0[2] = { 2, 1 };
const u8 gamepakWaitState1[2] = { 4, 1 };
const u8 gamepakWaitState2[2] = { 8, 1 };
const bool isInRom [16]=
  { false, false, false, false, false, false, false, false,
    true, true, true, true, true, true, false, false };              

u8 memoryWait[16] =
  { 0, 0, 2, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 0 };
u8 memoryWait32[16] =
  { 0, 0, 5, 0, 0, 1, 1, 0, 7, 7, 9, 9, 13, 13, 4, 0 };
u8 memoryWaitSeq[16] =
  { 0, 0, 2, 0, 0, 0, 0, 0, 2, 2, 4, 4, 8, 8, 4, 0 };
u8 memoryWaitSeq32[16] =
  { 0, 0, 5, 0, 0, 1, 1, 0, 5, 5, 9, 9, 17, 17, 4, 0 };

// The videoMemoryWait constants are used to add some waitstates
// if the opcode access video memory data outside of vblank/hblank
// It seems to happen on only one ticks for each pixel.
// Not used for now (too problematic with current code).
//const u8 videoMemoryWait[16] =
//  {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};


u8 biosProtected[4];

#ifdef WORDS_BIGENDIAN
bool cpuBiosSwapped = false;
#endif

u32 myROM[] = {
0xEA000006,
0xEA000093,
0xEA000006,
0x00000000,
0x00000000,
0x00000000,
0xEA000088,
0x00000000,
0xE3A00302,
0xE1A0F000,
0xE92D5800,
0xE55EC002,
0xE28FB03C,
0xE79BC10C,
0xE14FB000,
0xE92D0800,
0xE20BB080,
0xE38BB01F,
0xE129F00B,
0xE92D4004,
0xE1A0E00F,
0xE12FFF1C,
0xE8BD4004,
0xE3A0C0D3,
0xE129F00C,
0xE8BD0800,
0xE169F00B,
0xE8BD5800,
0xE1B0F00E,
0x0000009C,
0x0000009C,
0x0000009C,
0x0000009C,
0x000001F8,
0x000001F0,
0x000000AC,
0x000000A0,
0x000000FC,
0x00000168,
0xE12FFF1E,
0xE1A03000,
0xE1A00001,
0xE1A01003,
0xE2113102,
0x42611000,
0xE033C040,
0x22600000,
0xE1B02001,
0xE15200A0,
0x91A02082,
0x3AFFFFFC,
0xE1500002,
0xE0A33003,
0x20400002,
0xE1320001,
0x11A020A2,
0x1AFFFFF9,
0xE1A01000,
0xE1A00003,
0xE1B0C08C,
0x22600000,
0x42611000,
0xE12FFF1E,
0xE92D0010,
0xE1A0C000,
0xE3A01001,
0xE1500001,
0x81A000A0,
0x81A01081,
0x8AFFFFFB,
0xE1A0000C,
0xE1A04001,
0xE3A03000,
0xE1A02001,
0xE15200A0,
0x91A02082,
0x3AFFFFFC,
0xE1500002,
0xE0A33003,
0x20400002,
0xE1320001,
0x11A020A2,
0x1AFFFFF9,
0xE0811003,
0xE1B010A1,
0xE1510004,
0x3AFFFFEE,
0xE1A00004,
0xE8BD0010,
0xE12FFF1E,
0xE0010090,
0xE1A01741,
0xE2611000,
0xE3A030A9,
0xE0030391,
0xE1A03743,
0xE2833E39,
0xE0030391,
0xE1A03743,
0xE2833C09,
0xE283301C,
0xE0030391,
0xE1A03743,
0xE2833C0F,
0xE28330B6,
0xE0030391,
0xE1A03743,
0xE2833C16,
0xE28330AA,
0xE0030391,
0xE1A03743,
0xE2833A02,
0xE2833081,
0xE0030391,
0xE1A03743,
0xE2833C36,
0xE2833051,
0xE0030391,
0xE1A03743,
0xE2833CA2,
0xE28330F9,
0xE0000093,
0xE1A00840,
0xE12FFF1E,
0xE3A00001,
0xE3A01001,
0xE92D4010,
0xE3A03000,
0xE3A04001,
0xE3500000,
0x1B000004,
0xE5CC3301,
0xEB000002,
0x0AFFFFFC,
0xE8BD4010,
0xE12FFF1E,
0xE3A0C301,
0xE5CC3208,
0xE15C20B8,
0xE0110002,
0x10222000,
0x114C20B8,
0xE5CC4208,
0xE12FFF1E,
0xE92D500F,
0xE3A00301,
0xE1A0E00F,
0xE510F004,
0xE8BD500F,
0xE25EF004,
0xE59FD044,
0xE92D5000,
0xE14FC000,
0xE10FE000,
0xE92D5000,
0xE3A0C302,
0xE5DCE09C,
0xE35E00A5,
0x1A000004,
0x05DCE0B4,
0x021EE080,
0xE28FE004,
0x159FF018,
0x059FF018,
0xE59FD018,
0xE8BD5000,
0xE169F00C,
0xE8BD5000,
0xE25EF004,
0x03007FF0,
0x09FE2000,
0x09FFC000,
0x03007FE0
};
/*
variable_desc saveGameStruct[] = {
  { &DISPCNT  , sizeof(u16) },
  { &DISPSTAT , sizeof(u16) },
  { &VCOUNT   , sizeof(u16) },
  { &BG0CNT   , sizeof(u16) },
  { &BG1CNT   , sizeof(u16) },
  { &BG2CNT   , sizeof(u16) },
  { &BG3CNT   , sizeof(u16) },
  { &BG0HOFS  , sizeof(u16) },
  { &BG0VOFS  , sizeof(u16) },
  { &BG1HOFS  , sizeof(u16) },
  { &BG1VOFS  , sizeof(u16) },
  { &BG2HOFS  , sizeof(u16) },
  { &BG2VOFS  , sizeof(u16) },
  { &BG3HOFS  , sizeof(u16) },
  { &BG3VOFS  , sizeof(u16) },
  { &BG2PA    , sizeof(u16) },
  { &BG2PB    , sizeof(u16) },
  { &BG2PC    , sizeof(u16) },
  { &BG2PD    , sizeof(u16) },
  { &BG2X_L   , sizeof(u16) },
  { &BG2X_H   , sizeof(u16) },
  { &BG2Y_L   , sizeof(u16) },
  { &BG2Y_H   , sizeof(u16) },
  { &BG3PA    , sizeof(u16) },
  { &BG3PB    , sizeof(u16) },
  { &BG3PC    , sizeof(u16) },
  { &BG3PD    , sizeof(u16) },
  { &BG3X_L   , sizeof(u16) },
  { &BG3X_H   , sizeof(u16) },
  { &BG3Y_L   , sizeof(u16) },
  { &BG3Y_H   , sizeof(u16) },
  { &WIN0H    , sizeof(u16) },
  { &WIN1H    , sizeof(u16) },
  { &WIN0V    , sizeof(u16) },
  { &WIN1V    , sizeof(u16) },
  { &WININ    , sizeof(u16) },
  { &WINOUT   , sizeof(u16) },
  { &MOSAIC   , sizeof(u16) },
  { &BLDMOD   , sizeof(u16) },
  { &COLEV    , sizeof(u16) },
  { &COLY     , sizeof(u16) },
  { &DM0SAD_L , sizeof(u16) },
  { &DM0SAD_H , sizeof(u16) },
  { &DM0DAD_L , sizeof(u16) },
  { &DM0DAD_H , sizeof(u16) },
  { &DM0CNT_L , sizeof(u16) },
  { &DM0CNT_H , sizeof(u16) },
  { &DM1SAD_L , sizeof(u16) },
  { &DM1SAD_H , sizeof(u16) },
  { &DM1DAD_L , sizeof(u16) },
  { &DM1DAD_H , sizeof(u16) },
  { &DM1CNT_L , sizeof(u16) },
  { &DM1CNT_H , sizeof(u16) },
  { &DM2SAD_L , sizeof(u16) },
  { &DM2SAD_H , sizeof(u16) },
  { &DM2DAD_L , sizeof(u16) },
  { &DM2DAD_H , sizeof(u16) },
  { &DM2CNT_L , sizeof(u16) },
  { &DM2CNT_H , sizeof(u16) },
  { &DM3SAD_L , sizeof(u16) },
  { &DM3SAD_H , sizeof(u16) },
  { &DM3DAD_L , sizeof(u16) },
  { &DM3DAD_H , sizeof(u16) },
  { &DM3CNT_L , sizeof(u16) },
  { &DM3CNT_H , sizeof(u16) },
  { &TM0D     , sizeof(u16) },
  { &TM0CNT   , sizeof(u16) },
  { &TM1D     , sizeof(u16) },
  { &TM1CNT   , sizeof(u16) },
  { &TM2D     , sizeof(u16) },
  { &TM2CNT   , sizeof(u16) },
  { &TM3D     , sizeof(u16) },
  { &TM3CNT   , sizeof(u16) },
  { &P1       , sizeof(u16) },
  { &IE       , sizeof(u16) },
  { &IF       , sizeof(u16) },
  { &IME      , sizeof(u16) },
  { &holdState, sizeof(bool) },
  { &holdType, sizeof(int) },
  { &lcdTicks, sizeof(int) },
  { &timer0On , sizeof(bool) },
  { &timer0Ticks , sizeof(int) },
  { &timer0Reload , sizeof(int) },
  { &timer0ClockReload  , sizeof(int) },
  { &timer1On , sizeof(bool) },
  { &timer1Ticks , sizeof(int) },
  { &timer1Reload , sizeof(int) },
  { &timer1ClockReload  , sizeof(int) },
  { &timer2On , sizeof(bool) },
  { &timer2Ticks , sizeof(int) },
  { &timer2Reload , sizeof(int) },
  { &timer2ClockReload  , sizeof(int) },
  { &timer3On , sizeof(bool) },
  { &timer3Ticks , sizeof(int) },
  { &timer3Reload , sizeof(int) },
  { &timer3ClockReload  , sizeof(int) },
  { &dma0Source , sizeof(u32) },
  { &dma0Dest , sizeof(u32) },
  { &dma1Source , sizeof(u32) },
  { &dma1Dest , sizeof(u32) },
  { &dma2Source , sizeof(u32) },
  { &dma2Dest , sizeof(u32) },
  { &dma3Source , sizeof(u32) },
  { &dma3Dest , sizeof(u32) },
  { &fxOn, sizeof(bool) },
  { &windowOn, sizeof(bool) },
  { &N_FLAG , sizeof(bool) },
  { &C_FLAG , sizeof(bool) },
  { &Z_FLAG , sizeof(bool) },
  { &V_FLAG , sizeof(bool) },
  { &armState , sizeof(bool) },
  { &armIrqEnable , sizeof(bool) },
  { &armNextPC , sizeof(u32) },
  { &armMode , sizeof(int) },
  { &saveType , sizeof(int) },
  { NULL, 0 } 
};
*/
int romSize = 0x200000; //test normal 0x2000000 current 1/10 oh no only 2.4 MB

#ifdef PROFILING
void cpuProfil(profile_segment *seg)
{
    profilSegment = seg;
}

void cpuEnableProfiling(int hz)
{
  if(hz == 0)
    hz = 100;
  profilingTicks = profilingTicksReload = 16777216 / hz;
  profSetHertz(hz);
}
#endif


static bool CPUWriteState(gzFile file)
{
  utilWriteInt(file, SAVE_GAME_VERSION);

  utilGzWrite(file, &rom[0xa0], 16);

  utilWriteInt(file, useBios);
  
  utilGzWrite(file, &reg[0], sizeof(reg));

  //utilWriteData(file, saveGameStruct); //todo

  // new to version 0.7.1
  utilWriteInt(file, stopState);
  // new to version 0.8
  utilWriteInt(file, IRQTicks);

  utilGzWrite(file, internalRAM, 0x8000);
  utilGzWrite(file, paletteRAM, 0x400);
  utilGzWrite(file, workRAM, 0x40000);
  utilGzWrite(file, vram, 0x20000);
  utilGzWrite(file, oam, 0x400);
  utilGzWrite(file, pix, 4*241*162);
  utilGzWrite(file, ioMem, 0x400);

  eepromSaveGame(file);
  flashSaveGame(file);
  //soundSaveGame(file);

  //cheatsSaveGame(file);

  // version 1.5
  rtcSaveGame(file);
  
  return true;
}

bool CPUWriteState(const char *file)
{
  gzFile file2 = utilGzOpen(file, "wb");

  if(file2 == NULL) {
    systemMessage(MSG_ERROR_CREATING_FILE, N_("Error creating file %s"), file);
    return false;
  }
  
  bool res = CPUWriteState(file2);

  utilGzClose(file2);
  
  return res;
}

bool CPUWriteMemState(char *memory, int available)
{
  gzFile file2 = utilMemGzOpen(memory, available, "w");

  if(file2 == NULL) {
    return false;
  }

  bool res = CPUWriteState(file2);

  long pos = utilGzMemTell(file2)+8;

  if(pos >= (available))
    res = false;

  utilGzClose(file2);

  return res;
}

static bool CPUReadState(gzFile file)
{
  int version = utilReadInt(file);

  if(version > SAVE_GAME_VERSION || version < SAVE_GAME_VERSION_1) {
    systemMessage(MSG_UNSUPPORTED_VBA_SGM,
                  N_("Unsupported VisualBoyAdvance save game version %d"),
                  version);
    return false;
  }
  
  u8 romname[17];

  utilGzRead(file, romname, 16);

  if(memcmp(&rom[0xa0], romname, 16) != 0) {
    romname[16]=0;
    for(int i = 0; i < 16; i++)
      if(romname[i] < 32)
        romname[i] = 32;
    systemMessage(MSG_CANNOT_LOAD_SGM, N_("Cannot load save game for %s"), romname);
    return false;
  }

  bool ub = utilReadInt(file) ? true : false;

  if(ub != useBios) {
    if(useBios)
      systemMessage(MSG_SAVE_GAME_NOT_USING_BIOS,
                    N_("Save game is not using the BIOS files"));
    else
      systemMessage(MSG_SAVE_GAME_USING_BIOS,
                    N_("Save game is using the BIOS file"));
    return false;
  }

  utilGzRead(file, &reg[0], sizeof(reg));

  //utilReadData(file, saveGameStruct); //todo

  if(version < SAVE_GAME_VERSION_3)
    stopState = false;
  else
    stopState = utilReadInt(file) ? true : false;

  if(version < SAVE_GAME_VERSION_4)
  {
    IRQTicks = 0;
    intState = false;
  }
  else
  {
    IRQTicks = utilReadInt(file);
    if (IRQTicks>0)
      intState = true;
    else
    {
      intState = false;
      IRQTicks = 0;
    }
  }
  
  utilGzRead(file, internalRAM, 0x8000);
  utilGzRead(file, paletteRAM, 0x400);
  utilGzRead(file, workRAM, 0x40000);
  utilGzRead(file, vram, 0x20000);
  utilGzRead(file, oam, 0x400);
  if(version < SAVE_GAME_VERSION_6)
    utilGzRead(file, pix, 4*240*160);
  else
    utilGzRead(file, pix, 4*241*162);
  utilGzRead(file, ioMem, 0x400);

  eepromReadGame(file, version);
  flashReadGame(file, version);
  //soundReadGame(file, version); //ichfly this is not working
  
  if(version > SAVE_GAME_VERSION_1) {
    //cheatsReadGame(file, version);
  }
  if(version > SAVE_GAME_VERSION_6) {
    rtcReadGame(file);
  }

  if(version <= SAVE_GAME_VERSION_7) {
    u32 temp;
#define SWAP(a,b,c) \
    temp = (a);\
    (a) = (b)<<16|(c);\
    (b) = (temp) >> 16;\
    (c) = (temp) & 0xFFFF;
    
    SWAP(dma0Source, DM0SAD_H, DM0SAD_L);
    SWAP(dma0Dest,   DM0DAD_H, DM0DAD_L);
    SWAP(dma1Source, DM1SAD_H, DM1SAD_L);
    SWAP(dma1Dest,   DM1DAD_H, DM1DAD_L);
    SWAP(dma2Source, DM2SAD_H, DM2SAD_L);
    SWAP(dma2Dest,   DM2DAD_H, DM2DAD_L);
    SWAP(dma3Source, DM3SAD_H, DM3SAD_L);
    SWAP(dma3Dest,   DM3DAD_H, DM3DAD_L);
  }

  if(version <= SAVE_GAME_VERSION_8) {
    timer0ClockReload = TIMER_TICKS[TM0CNT & 3]; 
    timer1ClockReload = TIMER_TICKS[TM1CNT & 3];
    timer2ClockReload = TIMER_TICKS[TM2CNT & 3];
    timer3ClockReload = TIMER_TICKS[TM3CNT & 3];

    timer0Ticks = ((0x10000 - TM0D) << timer0ClockReload) - timer0Ticks;
    timer1Ticks = ((0x10000 - TM1D) << timer1ClockReload) - timer1Ticks;
    timer2Ticks = ((0x10000 - TM2D) << timer2ClockReload) - timer2Ticks;
    timer3Ticks = ((0x10000 - TM3D) << timer3ClockReload) - timer3Ticks;
  }

  // set pointers!
  //layerEnable = layerSettings & DISPCNT;
  
  //CPUUpdateRender();
  //CPUUpdateRenderBuffers(true);
  //CPUUpdateWindow0();
  //CPUUpdateWindow1();
  gbaSaveType = 0;
  switch(saveType) {
  case 0:
    cpuSaveGameFunc = flashSaveDecide;
    break;
  case 1:
    cpuSaveGameFunc = sramWrite;
    gbaSaveType = 1;
    break;
  case 2:
    cpuSaveGameFunc = flashWrite;
    gbaSaveType = 2;
    break;
  case 3:
     break;
  case 5:
    gbaSaveType = 5;
    break;
  default:
    systemMessage(MSG_UNSUPPORTED_SAVE_TYPE,
                  N_("Unsupported save type %d"), saveType);
    break;
  }
  if(eepromInUse)
    gbaSaveType = 3;

  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
  if(armState) {
    ARM_PREFETCH;
  } else {
    THUMB_PREFETCH;
  }

  CPUUpdateRegister(0x204, CPUReadHalfWordQuick(0x4000204));
  
  return true;  
}

bool CPUReadMemState(char *memory, int available)
{
  gzFile file2 = utilMemGzOpen(memory, available, "r");

  bool res = CPUReadState(file2);

  utilGzClose(file2);

  return res;
}

bool CPUReadState(const char * file)
{
  gzFile file2 = utilGzOpen(file, "rb");

  if(file2 == NULL)
    return false;
  
  bool res = CPUReadState(file2);

  utilGzClose(file2);

  return res;
}

bool CPUExportEepromFile(const char *fileName)
{
  if(eepromInUse) {
    FILE *file = fopen(fileName, "wb");
    
    if(!file) {
      systemMessage(MSG_ERROR_CREATING_FILE, N_("Error creating file %s"),
                    fileName);
      return false;
    }

    for(int i = 0; i < eepromSize;) {
      for(int j = 0; j < 8; j++) {
        if(fwrite(&eepromData[i+7-j], 1, 1, file) != 1) {
          fclose(file);
          return false;
        }
      }
      i += 8;
    }
    fclose(file);
  }
  return true;
}

bool CPUWriteBatteryFile(const char *fileName)
{
  if(gbaSaveType == 0) {
    if(eepromInUse)
      gbaSaveType = 3;
    else switch(saveType) {
    case 1:
      gbaSaveType = 1;
      break;
    case 2:
      gbaSaveType = 2;
      break;
    }
  }
  
  if((gbaSaveType) && (gbaSaveType!=5)) {
    FILE *file = fopen(fileName, "wb");
    
    if(!file) {
      systemMessage(MSG_ERROR_CREATING_FILE, N_("Error creating file %s"),
                    fileName);
      return false;
    }
    
    // only save if Flash/Sram in use or EEprom in use
    if(gbaSaveType != 3) {
      if(gbaSaveType == 2) {
        if(fwrite(flashSaveMemory, 1, flashSize, file) != (size_t)flashSize) {
          fclose(file);
          return false;
        }
      } else {
        if(fwrite(flashSaveMemory, 1, 0x10000, file) != 0x10000) {
          fclose(file);
          return false;
        }
      }
    } else {
      if(fwrite(eepromData, 1, eepromSize, file) != (size_t)eepromSize) {
        fclose(file);
        return false;
      }
    }
    fclose(file);
  }
  return true;
}
bool CPUImportEepromFile(const char *fileName)
{
  FILE *file = fopen(fileName, "rb");
    
  if(!file)
    return false;
  
  // check file size to know what we should read
  fseek(file, 0, SEEK_END);

  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  if(size == 512 || size == 0x2000) {
    if(fread(eepromData, 1, size, file) != (size_t)size) {
      fclose(file);
      return false;
    }
    for(int i = 0; i < size;) {
      u8 tmp = eepromData[i];
      eepromData[i] = eepromData[7-i];
      eepromData[7-i] = tmp;
      i++;
      tmp = eepromData[i];
      eepromData[i] = eepromData[7-i];
      eepromData[7-i] = tmp;
      i++;
      tmp = eepromData[i];
      eepromData[i] = eepromData[7-i];
      eepromData[7-i] = tmp;
      i++;      
      tmp = eepromData[i];
      eepromData[i] = eepromData[7-i];
      eepromData[7-i] = tmp;
      i++;      
      i += 4;
    }
  } else
    return false;
  fclose(file);
  return true;
}

bool CPUReadBatteryFile(const char *fileName)
{
  FILE *file = fopen(fileName, "rb");
    
  if(!file)
    return false;
  
  // check file size to know what we should read
  fseek(file, 0, SEEK_END);

  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

  if(size == 512 || size == 0x2000) {
    if(fread(eepromData, 1, size, file) != (size_t)size) {
      fclose(file);
      return false;
    }
  } else {
    if(size == 0x20000) {
      if(fread(flashSaveMemory, 1, 0x20000, file) != 0x20000) {
        fclose(file);
        return false;
      }
      flashSetSize(0x20000);
    } else {
      if(fread(flashSaveMemory, 1, 0x10000, file) != 0x10000) {
        fclose(file);
        return false;
      }
      flashSetSize(0x10000);
    }
  }
  fclose(file);
  return true;
}

bool CPUWritePNGFile(const char *fileName)
{
  //return utilWritePNGFile(fileName, 240, 160, pix); //todo
}

bool CPUWriteBMPFile(const char *fileName)
{
  //return utilWriteBMPFile(fileName, 240, 160, pix);
}

bool CPUIsZipFile(const char * file)
{
  if(strlen(file) > 4) {
    const char * p = strrchr(file,'.');

    if(p != NULL) {
      if(_stricmp(p, ".zip") == 0)
        return true;
    }
  }

  return false;
}

/*bool CPUIsGBAImage(const char * file)
{
  cpuIsMultiBoot = false;
  if(strlen(file) > 4) {
    const char * p = strrchr(file,'.');

    if(p != NULL) {
      if(_stricmp(p, ".gba") == 0)
        return true;
      if(_stricmp(p, ".agb") == 0)
        return true;
      if(_stricmp(p, ".bin") == 0)
        return true;
      if(_stricmp(p, ".elf") == 0)
        return true;
      if(_stricmp(p, ".mb") == 0) {
        cpuIsMultiBoot = true;
        return true;
      }
    }
  }

  return false;
}*/

bool CPUIsGBABios(const char * file)
{
  if(strlen(file) > 4) {
    const char * p = strrchr(file,'.');

    if(p != NULL) {
      if(_stricmp(p, ".gba") == 0)
        return true;
      if(_stricmp(p, ".agb") == 0)
        return true;
      if(_stricmp(p, ".bin") == 0)
        return true;
      if(_stricmp(p, ".bios") == 0)
        return true;
    }
  }
  
  return false;
}

bool CPUIsELF(const char *file)
{
  if(strlen(file) > 4) {
    const char * p = strrchr(file,'.');
    
    if(p != NULL) {
      if(_stricmp(p, ".elf") == 0)
        return true;
    }
  }
  return false;
}

void CPUCleanUp()
{
#ifdef PROFILING
  if(profilingTicksReload) {
    profCleanup();
  }
#endif
  
  if(rom != NULL) {
    free(rom);
    rom = NULL;
  }

  if(vram != NULL) {
    free(vram);
    vram = NULL;
  }

  if(paletteRAM != NULL) {
    free(paletteRAM);
    paletteRAM = NULL;
  }
  
  if(internalRAM != NULL) {
    free(internalRAM);
    internalRAM = NULL;
  }

  if(workRAM != NULL) {
    free(workRAM);
    workRAM = NULL;
  }

  if(bios != NULL) {
    free(bios);
    bios = NULL;
  }

  if(pix != NULL) {
    free(pix);
    pix = NULL;
  }

  if(oam != NULL) {
    free(oam);
    oam = NULL;
  }

  if(ioMem != NULL) {
    free(ioMem);
    ioMem = NULL;
  }
  
  //elfCleanUp();

  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

  emulating = 0;
}

int CPULoadRom(const char *szFile,bool extram)
{


  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
  
#ifdef loadindirect

	  romSize = 0x02400000 - ((u32)getHeapEnd() + 0x5000 + 0x7000);
	  rom = (u8 *)(getHeapEnd() + 0x5000/*for the other malloc here*/ + 0x7000/*28K for futur alloc*/);              //rom = (u8 *)0x02180000; //old
#endif
  workRAM = (u8*)0x02000000;/*(u8 *)calloc(1, 0x40000);
  if(workRAM == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "WRAM");
    return 0;
  }*/
#ifdef loadindirect
  u8 *whereToLoad = rom;
  if(cpuIsMultiBoot)
    whereToLoad = workRAM;
		if(!utilLoad(szFile,
						  utilIsGBAImage,
						  whereToLoad,
						  romSize,extram))
		{
			return 0;
		}
#else
rom = (u8*)puzzleorginal_bin;  //rom = (u8*)puzzleorginal_bin;
#endif

  bios = (u8 *)calloc(1,0x4000);
  if(bios == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "BIOS");
    CPUCleanUp();
    return 0;
  }    
  internalRAM = (u8 *)0x03000000;//calloc(1,0x8000);
  /*if(internalRAM == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "IRAM");
    CPUCleanUp();
    return 0;
  }*/
  paletteRAM = (u8 *)0x05000000;//calloc(1,0x400);
  /*if(paletteRAM == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "PRAM");
    CPUCleanUp();
    return 0;
  }*/      
  vram = (u8 *)0x06000000;//calloc(1, 0x20000);
  /*if(vram == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "VRAM");
    CPUCleanUp();
    return 0;
  }*/      
  oam = (u8 *)0x07000000;/*calloc(1, 0x400); //ichfly test
  if(oam == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "OAM");
    CPUCleanUp();
    return 0;
  }      
  pix = (u8 *)calloc(1, 4 * 241 * 162);
  if(pix == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "PIX");
    CPUCleanUp();
    return 0;
  }  */
  ioMem = (u8 *)calloc(1, 0x400);
  if(ioMem == NULL) {
    systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                  "IO");
    CPUCleanUp();
    return 0;
  }      

  flashInit();
  eepromInit();

  //CPUUpdateRenderBuffers(true);

  return romSize;
}
void CPUUpdateCPSR()
{
  u32 CPSR = reg[16].I & 0x40;
  if(N_FLAG)
    CPSR |= 0x80000000;
  if(Z_FLAG)
    CPSR |= 0x40000000;
  if(C_FLAG)
    CPSR |= 0x20000000;
  if(V_FLAG)
    CPSR |= 0x10000000;
  if(!armState)
    CPSR |= 0x00000020;
  if(!armIrqEnable)
    CPSR |= 0x80;
  CPSR |= (armMode & 0x1F);
  reg[16].I = CPSR;
}

void CPUUpdateFlags(bool breakLoop)
{
  u32 CPSR = reg[16].I;
  
  N_FLAG = (CPSR & 0x80000000) ? true: false;
  Z_FLAG = (CPSR & 0x40000000) ? true: false;
  C_FLAG = (CPSR & 0x20000000) ? true: false;
  V_FLAG = (CPSR & 0x10000000) ? true: false;
  armState = (CPSR & 0x20) ? false : true;
  armIrqEnable = (CPSR & 0x80) ? false : true;
  if(breakLoop) {
      if (armIrqEnable && (IF & IE) && (IME & 1))
        cpuNextEvent = cpuTotalTicks;
  }
}

void CPUUpdateFlags()
{
  CPUUpdateFlags(true);
}

#ifdef WORDS_BIGENDIAN
static void CPUSwap(volatile u32 *a, volatile u32 *b)
{
  volatile u32 c = *b;
  *b = *a;
  *a = c;
}
#else
static void CPUSwap(u32 *a, u32 *b)
{
  u32 c = *b;
  *b = *a;
  *a = c;
}
#endif

void CPUSwitchMode(int mode, bool saveState, bool breakLoop)
{
  //  if(armMode == mode)
  //    return;
  
  CPUUpdateCPSR();

  switch(armMode) {
  case 0x10:
  case 0x1F:
    reg[R13_USR].I = reg[13].I;
    reg[R14_USR].I = reg[14].I;
    reg[17].I = reg[16].I;
    break;
  case 0x11:
    CPUSwap(&reg[R8_FIQ].I, &reg[8].I);
    CPUSwap(&reg[R9_FIQ].I, &reg[9].I);
    CPUSwap(&reg[R10_FIQ].I, &reg[10].I);
    CPUSwap(&reg[R11_FIQ].I, &reg[11].I);
    CPUSwap(&reg[R12_FIQ].I, &reg[12].I);
    reg[R13_FIQ].I = reg[13].I;
    reg[R14_FIQ].I = reg[14].I;
    reg[SPSR_FIQ].I = reg[17].I;
    break;
  case 0x12:
    reg[R13_IRQ].I  = reg[13].I;
    reg[R14_IRQ].I  = reg[14].I;
    reg[SPSR_IRQ].I =  reg[17].I;
    break;
  case 0x13:
    reg[R13_SVC].I  = reg[13].I;
    reg[R14_SVC].I  = reg[14].I;
    reg[SPSR_SVC].I =  reg[17].I;
    break;
  case 0x17:
    reg[R13_ABT].I  = reg[13].I;
    reg[R14_ABT].I  = reg[14].I;
    reg[SPSR_ABT].I =  reg[17].I;
    break;
  case 0x1b:
    reg[R13_UND].I  = reg[13].I;
    reg[R14_UND].I  = reg[14].I;
    reg[SPSR_UND].I =  reg[17].I;
    break;
  }

  u32 CPSR = reg[16].I;
  u32 SPSR = reg[17].I;
  
  switch(mode) {
  case 0x10:
  case 0x1F:
    reg[13].I = reg[R13_USR].I;
    reg[14].I = reg[R14_USR].I;
    reg[16].I = SPSR;
    break;
  case 0x11:
    CPUSwap(&reg[8].I, &reg[R8_FIQ].I);
    CPUSwap(&reg[9].I, &reg[R9_FIQ].I);
    CPUSwap(&reg[10].I, &reg[R10_FIQ].I);
    CPUSwap(&reg[11].I, &reg[R11_FIQ].I);
    CPUSwap(&reg[12].I, &reg[R12_FIQ].I);
    reg[13].I = reg[R13_FIQ].I;
    reg[14].I = reg[R14_FIQ].I;
    if(saveState)
      reg[17].I = CPSR;
    else
      reg[17].I = reg[SPSR_FIQ].I;
    break;
  case 0x12:
    reg[13].I = reg[R13_IRQ].I;
    reg[14].I = reg[R14_IRQ].I;
    reg[16].I = SPSR;
    if(saveState)
      reg[17].I = CPSR;
    else
      reg[17].I = reg[SPSR_IRQ].I;
    break;
  case 0x13:
    reg[13].I = reg[R13_SVC].I;
    reg[14].I = reg[R14_SVC].I;
    reg[16].I = SPSR;
    if(saveState)
      reg[17].I = CPSR;
    else
      reg[17].I = reg[SPSR_SVC].I;
    break;
  case 0x17:
    reg[13].I = reg[R13_ABT].I;
    reg[14].I = reg[R14_ABT].I;
    reg[16].I = SPSR;
    if(saveState)
      reg[17].I = CPSR;
    else
      reg[17].I = reg[SPSR_ABT].I;
    break;    
  case 0x1b:
    reg[13].I = reg[R13_UND].I;
    reg[14].I = reg[R14_UND].I;
    reg[16].I = SPSR;
    if(saveState)
      reg[17].I = CPSR;
    else
      reg[17].I = reg[SPSR_UND].I;
    break;    
  default:
    systemMessage(MSG_UNSUPPORTED_ARM_MODE, N_("Unsupported ARM mode %02x"), mode);
    break;
  }
  armMode = mode;
  CPUUpdateFlags(breakLoop);
  CPUUpdateCPSR();
}

void CPUSwitchMode(int mode, bool saveState)
{
  CPUSwitchMode(mode, saveState, true);
}
void doDMAslow(u32 &s, u32 &d, u32 si, u32 di, u32 c, int transfer32) //ichfly veraltet
{

  if(transfer32) {
    s &= 0xFFFFFFFC;
    if(s < 0x02000000 && (reg[15].I >> 24)) {
      while(c != 0) {
        CPUWriteMemory(d, 0);
        d += di;
        c--;
      }
    } else {
      while(c != 0) {
        cpuDmaLast = CPUReadMemory(s);
        CPUWriteMemory(d, cpuDmaLast);
        d += di;
        s += si;
        c--;
      }
    }
  } else {
    s &= 0xFFFFFFFE;
    si = (int)si >> 1;
    di = (int)di >> 1;
    if(s < 0x02000000 && (reg[15].I >> 24)) {
      while(c != 0) {
        CPUWriteHalfWord(d, 0);
        d += di;
        c--;
      }
    } else {
      while(c != 0) {
        cpuDmaLast = CPUReadHalfWord(s);
        CPUWriteHalfWord(d, cpuDmaLast);
        cpuDmaLast |= (cpuDmaLast<<16);
        d += di;
        s += si;
        c--;
      }
    }
  }

}
void doDMA(u32 &s, u32 &d, u32 si, u32 di, u32 c, int transfer32) //ichfly veraltet
{
	if(si == 0 || di == 0 || s < 0x02000000 || d < 0x02000000 || (d & ~0xFFFFFF) == 0x04000000 || (s & ~0xFFFFFF) == 0x04000000 || s > 0x0E000000 || d > 0x0E000000)
	{
		doDMAslow(s, d, si, di, c, transfer32); //some checks
	}
	else
	{
		if(s & 0x08000000)
		{
#ifdef uppern_read_emulation

			if(((s&0x1FFFFFF) + c*4) > romSize) //slow
			{
				//iprintf("highdmaread %08X %08X %08X %08X %08X %X\r\n",s,d,c,si,di,transfer32);
				if(di == -4 || si == -4)//this can't work the slow way so use the
				{
					doDMAslow(s, d, si, di, c, transfer32); //very slow way
				}
				if(transfer32)
				{
					//doDMAslow(s, d, si, di, c, transfer32);
					fseek (ichflyfilestream , (s&0x1FFFFFF) , SEEK_SET);
					//iprintf("seek %08X\r\n",s&0x1FFFFFF);
					int dkdkdkdk = fread ((void*)d,1,c * 4,ichflyfilestream); // fist is buggy
					//iprintf("(%08X %08X %08X) ret %08X\r\n",d,c,ichflyfilestream,dkdkdkdk);
				}
				else
				{
					//iprintf("teeees");
					//doDMAslow(s, d, si, di, c, transfer32);
					fseek (ichflyfilestream , (s&0x1FFFFFF) , SEEK_SET);
					//iprintf("seek %08X\r\n",s&0x1FFFFFF);
					int dkdkdkdk = fread ((void*)d,1,c * 2,ichflyfilestream);
					//iprintf("(%08X %08X %08X) ret %08X\r\n",d,c,ichflyfilestream,dkdkdkdk);
				}
				//doDMAslow(s, d, si, di, c, transfer32); //very slow way
				return;
			}
			else
			{
				s = (u32)(rom +  (s & 0x01FFFFFF));
			}

#else
			s = (u32)(rom +  (s & 0x01FFFFFF));
#endif
		}
		while(dmaBusy(3)); // ichfly wait for dma 3
		DMA3_SRC = s;
		DMA3_DEST = d;

		int tmpzahl = DMA_ENABLE | c;
		if(transfer32)tmpzahl |= DMA_32_BIT;
		if(di == -4) tmpzahl |= DMA_DST_DEC;
		if(si == -4) tmpzahl |= DMA_SRC_DEC;
		DMA3_CR = tmpzahl;
		//iprintf("%x,%x,%x",s,d,tmpzahl);
	}

}

void CPUCheckDMA(int reason, int dmamask)
{
  // DMA 0
  if((DM0CNT_H & 0x8000) && (dmamask & 1)) {
    if(((DM0CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((DM0CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((DM0CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
#ifdef DEV_VERSION
      if(systemVerbose & VERBOSE_DMA0) {
        int count = (DM0CNT_L ? DM0CNT_L : 0x4000) << 1;
        if(DM0CNT_H & 0x0400)
          count <<= 1;
        iprintf("DMA0: s=%08x d=%08x c=%04x count=%08x\n", dma0Source, dma0Dest, 
            DM0CNT_H,
            count);
      }
#endif
      doDMA(dma0Source, dma0Dest, sourceIncrement, destIncrement,
            DM0CNT_L ? DM0CNT_L : 0x4000,
            DM0CNT_H & 0x0400);
      cpuDmaHack = true;

      if(DM0CNT_H & 0x4000) {
        IF |= 0x0100;
        UPDATE_REG(0x202, IF);
        cpuNextEvent = cpuTotalTicks;
      }
      
      if(((DM0CNT_H >> 5) & 3) == 3) {
        dma0Dest = DM0DAD_L | (DM0DAD_H << 16);
      }
      
      if(!(DM0CNT_H & 0x0200) || (reason == 0)) {
        DM0CNT_H &= 0x7FFF;
        UPDATE_REG(0xBA, DM0CNT_H);
      }
    }
  }
  
  // DMA 1
  if((DM1CNT_H & 0x8000) && (dmamask & 2)) {
    if(((DM1CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((DM1CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((DM1CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
      if(reason == 3) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_DMA1) {
          Log("DMA1: s=%08x d=%08x c=%04x count=%08x\n", dma1Source, dma1Dest,
              DM1CNT_H,
              16);
        }
#endif  
        doDMA(dma1Source, dma1Dest, sourceIncrement, 0, 4,
              0x0400);
      } else {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_DMA1) {
          int count = (DM1CNT_L ? DM1CNT_L : 0x4000) << 1;
          if(DM1CNT_H & 0x0400)
            count <<= 1;
          iprintf("DMA1: s=%08x d=%08x c=%04x count=%08x\n", dma1Source, dma1Dest,
              DM1CNT_H,
              count);
        }
#endif          
        doDMA(dma1Source, dma1Dest, sourceIncrement, destIncrement,
              DM1CNT_L ? DM1CNT_L : 0x4000,
              DM1CNT_H & 0x0400);
      }
      cpuDmaHack = true;

      if(DM1CNT_H & 0x4000) {
        IF |= 0x0200;
        UPDATE_REG(0x202, IF);
        cpuNextEvent = cpuTotalTicks;
      }
      
      if(((DM1CNT_H >> 5) & 3) == 3) {
        dma1Dest = DM1DAD_L | (DM1DAD_H << 16);
      }
      
      if(!(DM1CNT_H & 0x0200) || (reason == 0)) {
        DM1CNT_H &= 0x7FFF;
        UPDATE_REG(0xC6, DM1CNT_H);
      }
    }
  }
  
  // DMA 2
  if((DM2CNT_H & 0x8000) && (dmamask & 4)) {
    if(((DM2CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((DM2CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((DM2CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
      if(reason == 3) {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_DMA2) {
          int count = (4) << 2;
          Log("DMA2: s=%08x d=%08x c=%04x count=%08x\n", dma2Source, dma2Dest,
              DM2CNT_H,
              count);
        }
#endif                  
        doDMA(dma2Source, dma2Dest, sourceIncrement, 0, 4,
              0x0400);
      } else {
#ifdef DEV_VERSION
        if(systemVerbose & VERBOSE_DMA2) {
          int count = (DM2CNT_L ? DM2CNT_L : 0x4000) << 1;
          if(DM2CNT_H & 0x0400)
            count <<= 1;
          iprintf("DMA2: s=%08x d=%08x c=%04x count=%08x\n", dma2Source, dma2Dest,
              DM2CNT_H,
              count);
        }
#endif                  
        doDMA(dma2Source, dma2Dest, sourceIncrement, destIncrement,
              DM2CNT_L ? DM2CNT_L : 0x4000,
              DM2CNT_H & 0x0400);
      }
      cpuDmaHack = true;

      if(DM2CNT_H & 0x4000) {
        IF |= 0x0400;
        UPDATE_REG(0x202, IF);
        cpuNextEvent = cpuTotalTicks;
      }

      if(((DM2CNT_H >> 5) & 3) == 3) {
        dma2Dest = DM2DAD_L | (DM2DAD_H << 16);
      }
      
      if(!(DM2CNT_H & 0x0200) || (reason == 0)) {
        DM2CNT_H &= 0x7FFF;
        UPDATE_REG(0xD2, DM2CNT_H);
      }
    }
  }

  // DMA 3
  if((DM3CNT_H & 0x8000) && (dmamask & 8)) {
    if(((DM3CNT_H >> 12) & 3) == reason) {
      u32 sourceIncrement = 4;
      u32 destIncrement = 4;
      switch((DM3CNT_H >> 7) & 3) {
      case 0:
        break;
      case 1:
        sourceIncrement = (u32)-4;
        break;
      case 2:
        sourceIncrement = 0;
        break;
      }
      switch((DM3CNT_H >> 5) & 3) {
      case 0:
        break;
      case 1:
        destIncrement = (u32)-4;
        break;
      case 2:
        destIncrement = 0;
        break;
      }      
#ifdef DEV_VERSION
      if(systemVerbose & VERBOSE_DMA3) {
        int count = (DM3CNT_L ? DM3CNT_L : 0x10000) << 1;
        if(DM3CNT_H & 0x0400)
          count <<= 1;
        iprintf("DMA3: s=%08x d=%08x c=%04x count=%08x\n", dma3Source, dma3Dest,
            DM3CNT_H,
            count);
      }
#endif                
      doDMA(dma3Source, dma3Dest, sourceIncrement, destIncrement,
            DM3CNT_L ? DM3CNT_L : 0x10000,
            DM3CNT_H & 0x0400);
      if(DM3CNT_H & 0x4000) {
        IF |= 0x0800;
        UPDATE_REG(0x202, IF);
        cpuNextEvent = cpuTotalTicks;
      }

      if(((DM3CNT_H >> 5) & 3) == 3) {
        dma3Dest = DM3DAD_L | (DM3DAD_H << 16);
      }
      
      if(!(DM3CNT_H & 0x0200) || (reason == 0)) {
        DM3CNT_H &= 0x7FFF;
        UPDATE_REG(0xDE, DM3CNT_H);
      }
    }
  }
}

void CPUUpdateRegister(u32 address, u16 value)
{
  	/*if(0x60 > address && address > 0x7)
	{
			//iprintf("UP16 %x %x\r\n",address,value);
		    *(u16 *)((address & 0x3FF) + 0x4000000) = value;
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			//swiDelay(0x2000000);
			
	}*/

  switch(address) {
  case 0x00:
    {
	//iprintf("DISPCNT1 %x %x\r\n",&DISPCNT,DISPCNT);
	//DISPCNT = value;
	DISPCNT = value;//workaroundwrit16(value, (u16*)&DISPCNT);
	//iprintf("DISPCNT1 %x %x\r\n",&DISPCNT,DISPCNT);
	UPDATE_REG(0x00, DISPCNT);
	

    /*    if ((value & 7) >5)
          DISPCNT = (value &7);
      bool change = ((DISPCNT ^ value) & 0x80) ? true : false;
      bool changeBG = ((DISPCNT ^ value) & 0x0F00) ? true : false;
      u16 changeBGon = (((~DISPCNT) & value) & 0x0F00);
      DISPCNT = (value & 0xFFF7);
      UPDATE_REG(0x00, DISPCNT);

      if (changeBGon)
      {
         layerEnableDelay=4;
         layerEnable = layerSettings & value & (~changeBGon);
      }
       else
         layerEnable = layerSettings & value;
      //      CPUUpdateTicks();

      windowOn = (layerEnable & 0x6000) ? true : false;
      if(change && !((value & 0x80))) {
        if(!(DISPSTAT & 1)) {
          lcdTicks = 1008;
          //      VCOUNT = 0;
          //      UPDATE_REG(0x06, VCOUNT);
          DISPSTAT &= 0xFFFC;
          UPDATE_REG(0x04, DISPSTAT);
          CPUCompareVCOUNT();
        }
        //        (*renderLine)();
      }
      //CPUUpdateRender();
      // we only care about changes in BG0-BG3
      if(changeBG)
        CPUUpdateRenderBuffers(false);
		*/
		//iprintf("DISPCNT2 %x\r\n",DISPCNT);
		
    }
    break;
  case 0x04:
    DISPSTAT = (value & 0xFF38) | (DISPSTAT & 7);
    UPDATE_REG(0x04, DISPSTAT);
    break;
  case 0x06:
    // not writable
    break;
  case 0x08:
    BG0CNT = (value & 0xDFCF);
    UPDATE_REG(0x08, BG0CNT);
	*(u16 *)(0x4000008) = value;
    break;
  case 0x0A:
    BG1CNT = (value & 0xDFCF);
    UPDATE_REG(0x0A, BG1CNT);
    *(u16 *)(0x400000A) = value;
	break;
  case 0x0C:
    BG2CNT = (value & 0xFFCF);
    UPDATE_REG(0x0C, BG2CNT);
	if((DISPCNT & 7) < 3)*(u16 *)(0x400000C) = value;
	else //ichfly some extra handling 
	{
		REG_BG3CNT = REG_BG3CNT | (value & 0x43);
	}
    break;
  case 0x0E:
    BG3CNT = (value & 0xFFCF);
    UPDATE_REG(0x0E, BG3CNT);
	if((DISPCNT & 7) < 3)*(u16 *)(0x400000E) = value;
    break;
  case 0x10:
    BG0HOFS = value & 511;
    UPDATE_REG(0x10, BG0HOFS);
    *(u16 *)(0x4000010) = value;
	break;
  case 0x12:
    BG0VOFS = value & 511;
    UPDATE_REG(0x12, BG0VOFS);
    *(u16 *)(0x4000012) = value;
	break;
  case 0x14:
    BG1HOFS = value & 511;
    UPDATE_REG(0x14, BG1HOFS);
    break;
  case 0x16:
    BG1VOFS = value & 511;
    UPDATE_REG(0x16, BG1VOFS);
    *(u16 *)(0x4000016) = value;
	break;      
  case 0x18:
    BG2HOFS = value & 511;
    UPDATE_REG(0x18, BG2HOFS);
	*(u16 *)(0x4000018) = value;
    break;
  case 0x1A:
    BG2VOFS = value & 511;
    UPDATE_REG(0x1A, BG2VOFS);
    if((DISPCNT & 7) < 3)*(u16 *)(0x400001A) = value; //ichfly only update if it is save
	break;
  case 0x1C:
    BG3HOFS = value & 511;
    UPDATE_REG(0x1C, BG3HOFS);
    if((DISPCNT & 7) < 3)*(u16 *)(0x400001C) = value; //ichfly only update if it is save
	break;
  case 0x1E:
    BG3VOFS = value & 511;
    UPDATE_REG(0x1E, BG3VOFS);
    *(u16 *)(0x400001E) = value;
	break;      
  case 0x20:
    BG2PA = value;
    UPDATE_REG(0x20, BG2PA);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000020) = value;
	else *(u16 *)(0x4000030) = value;
	break;
  case 0x22:
    BG2PB = value;
    UPDATE_REG(0x22, BG2PB);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000022) = value;
	else *(u16 *)(0x4000032) = value;
	break;
  case 0x24:
    BG2PC = value;
    UPDATE_REG(0x24, BG2PC);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000024) = value;
	else *(u16 *)(0x4000034) = value;
	break;
  case 0x26:
    BG2PD = value;
    UPDATE_REG(0x26, BG2PD);
	if((DISPCNT & 7) < 3)*(u16 *)(0x4000026) = value;
	else *(u16 *)(0x4000036) = value;
	break;
  case 0x28:
    BG2X_L = value;
    UPDATE_REG(0x28, BG2X_L);
    //gfxBG2Changed |= 1;
	if((DISPCNT & 7) < 3)*(u16 *)(0x4000028) = value;
	else *(u16 *)(0x4000038) = value;
    break;
  case 0x2A:
    BG2X_H = (value & 0xFFF);
    UPDATE_REG(0x2A, BG2X_H);
    //gfxBG2Changed |= 1;
	if((DISPCNT & 7) < 3)*(u16 *)(0x400002A) = value;
	else *(u16 *)(0x400003A) = value;
    break;
  case 0x2C:
    BG2Y_L = value;
    UPDATE_REG(0x2C, BG2Y_L);
    //gfxBG2Changed |= 2;
	if((DISPCNT & 7) < 3)*(u16 *)(0x400002C) = value;
	else *(u16 *)(0x400003C) = value;
    break;
  case 0x2E:
    BG2Y_H = value & 0xFFF;
    UPDATE_REG(0x2E, BG2Y_H);
    //gfxBG2Changed |= 2;
	if((DISPCNT & 7) < 3)*(u16 *)(0x400002E) = value;
	else *(u16 *)(0x400003E) = value;
    break;
  case 0x30:
    BG3PA = value;
    UPDATE_REG(0x30, BG3PA);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000030) = value;
	break;
  case 0x32:
    BG3PB = value;
    UPDATE_REG(0x32, BG3PB);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000032) = value;
	break;
  case 0x34:
    BG3PC = value;
    UPDATE_REG(0x34, BG3PC);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000034) = value;
	break;
  case 0x36:
    BG3PD = value;
    UPDATE_REG(0x36, BG3PD);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000036) = value;
	break;
  case 0x38:
    BG3X_L = value;
    UPDATE_REG(0x38, BG3X_L);
    //gfxBG3Changed |= 1;
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000038) = value;
	break;
  case 0x3A:
    BG3X_H = value & 0xFFF;
    UPDATE_REG(0x3A, BG3X_H);
    //gfxBG3Changed |= 1;    
    if((DISPCNT & 7) < 3)*(u16 *)(0x400003A) = value;
	break;
  case 0x3C:
    BG3Y_L = value;
    UPDATE_REG(0x3C, BG3Y_L);
    //gfxBG3Changed |= 2;    
    if((DISPCNT & 7) < 3)*(u16 *)(0x400003C) = value;
	break;
  case 0x3E:
    BG3Y_H = value & 0xFFF;
    UPDATE_REG(0x3E, BG3Y_H);
    //gfxBG3Changed |= 2;    
    if((DISPCNT & 7) < 3)*(u16 *)(0x400003E) = value;
	break;
  case 0x40:
    WIN0H = value;
    UPDATE_REG(0x40, WIN0H);
    //CPUUpdateWindow0();
    *(u16 *)(0x4000040) = value;
	break;
  case 0x42:
    WIN1H = value;
    UPDATE_REG(0x42, WIN1H);
	*(u16 *)(0x4000042) = value;
    //CPUUpdateWindow1();    
    break;      
  case 0x44:
    WIN0V = value;
    UPDATE_REG(0x44, WIN0V);
    *(u16 *)(0x4000044) = value;
	break;
  case 0x46:
    WIN1V = value;
    UPDATE_REG(0x46, WIN1V);
    *(u16 *)(0x4000046) = value;
	break;
  case 0x48:
    WININ = value & 0x3F3F;
    UPDATE_REG(0x48, WININ);
    if((DISPCNT & 7) < 3)*(u16 *)(0x4000048) = value;
	else
	{
		int tempWININ = WININ & ~0x404;
		tempWININ = tempWININ | ((WININ & 0x404) << 1);
		WIN_IN = tempWININ;
	}
	break;
  case 0x4A:
    WINOUT = value & 0x3F3F;
    UPDATE_REG(0x4A, WINOUT);
    if((DISPCNT & 7) < 3)*(u16 *)(0x400004A) = value;
	else
	{
		int tempWINOUT = WINOUT & ~0x404;
		tempWINOUT = tempWINOUT | ((WINOUT & 0x404) << 1);
		WIN_OUT = tempWINOUT;
	}
	break;
  case 0x4C:
    MOSAIC = value;
    UPDATE_REG(0x4C, MOSAIC);
    *(u16 *)(0x400004C) = value;
	break;
  case 0x50:
    BLDMOD = value & 0x3FFF;
    UPDATE_REG(0x50, BLDMOD);
    //fxOn = ((BLDMOD>>6)&3) != 0;
    //CPUUpdateRender();
	if((DISPCNT & 7) < 3)*(u16 *)(0x4000050) = value;
	else
	{
		int tempBLDMOD = BLDMOD & ~0x404;
		tempBLDMOD = tempBLDMOD | ((BLDMOD & 0x404) << 1);
		REG_BLDCNT = tempBLDMOD;
	}
    break;
  case 0x52:
    COLEV = value & 0x1F1F;
    UPDATE_REG(0x52, COLEV);
    *(u16 *)(0x4000052) = value;
	break;
  case 0x54:
    COLY = value & 0x1F;
    UPDATE_REG(0x54, COLY);
	*(u16 *)(0x4000054) = value;
    break;
  case 0x60:
  case 0x62:
  case 0x64:
  case 0x68:
  case 0x6c:
  case 0x70:
  case 0x72:
  case 0x74:
  case 0x78:
  case 0x7c:
  case 0x80:
  case 0x84:
    /*soundEvent(address&0xFF, (u8)(value & 0xFF));
    soundEvent((address&0xFF)+1, (u8)(value>>8));
    break;*/ //ichfly disable sound
  case 0x82:
  case 0x88:
  case 0xa0:
  case 0xa2:
  case 0xa4:
  case 0xa6:
  case 0x90:
  case 0x92:
  case 0x94:
  case 0x96:
  case 0x98:
  case 0x9a:
  case 0x9c:
  case 0x9e:    
    //soundEvent(address&0xFF, value);  //ichfly send sound to arm7
#ifdef soundwriteprint
	  iprintf("ur %04x to %08x\r\n",value,address);
#endif
	  REG_IPC_FIFO_TX = (address | 0x80000000);
	  REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0
	  UPDATE_REG(address,value);
    break;
  case 0xB0:
    DM0SAD_L = value;
    UPDATE_REG(0xB0, DM0SAD_L);
    break;
  case 0xB2:
    DM0SAD_H = value & 0x07FF;
    UPDATE_REG(0xB2, DM0SAD_H);
    break;
  case 0xB4:
    DM0DAD_L = value;
    UPDATE_REG(0xB4, DM0DAD_L);
    break;
  case 0xB6:
    DM0DAD_H = value & 0x07FF;
    UPDATE_REG(0xB6, DM0DAD_H);
    break;
  case 0xB8:
    DM0CNT_L = value & 0x3FFF;
    UPDATE_REG(0xB8, 0);
    break;
  case 0xBA:
    {
      bool start = ((DM0CNT_H ^ value) & 0x8000) ? true : false;
      value &= 0xF7E0;

      DM0CNT_H = value;
      UPDATE_REG(0xBA, DM0CNT_H);    
    
      if(start && (value & 0x8000)) {
        dma0Source = DM0SAD_L | (DM0SAD_H << 16);
        dma0Dest = DM0DAD_L | (DM0DAD_H << 16);
        CPUCheckDMA(0, 1);
      }
    }
    break;      
  case 0xBC:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0
    
	DM1SAD_L = value;
    UPDATE_REG(0xBC, DM1SAD_L);
    break;
  case 0xBE:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

    DM1SAD_H = value & 0x0FFF;
    UPDATE_REG(0xBE, DM1SAD_H);
    break;
  case 0xC0:
#ifdef dmawriteprint
    iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

	DM1DAD_L = value;
    UPDATE_REG(0xC0, DM1DAD_L);
    break;
  case 0xC2:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

    DM1DAD_H = value & 0x07FF;
    UPDATE_REG(0xC2, DM1DAD_H);
    break;
  case 0xC4:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

	DM1CNT_L = value & 0x3FFF;
    UPDATE_REG(0xC4, 0);
    break;
  case 0xC6:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

	  {
      bool start = ((DM1CNT_H ^ value) & 0x8000) ? true : false;
      value &= 0xF7E0;
      
      DM1CNT_H = value;
      UPDATE_REG(0xC6, DM1CNT_H);
      
      if(start && (value & 0x8000)) {
        dma1Source = DM1SAD_L | (DM1SAD_H << 16);
        dma1Dest = DM1DAD_L | (DM1DAD_H << 16);
        CPUCheckDMA(0, 2);
      }
    }
    break;
  case 0xC8:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

	DM2SAD_L = value;
    UPDATE_REG(0xC8, DM2SAD_L);
    break;
  case 0xCA:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

	DM2SAD_H = value & 0x0FFF;
    UPDATE_REG(0xCA, DM2SAD_H);
    break;
  case 0xCC:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

	DM2DAD_L = value;
    UPDATE_REG(0xCC, DM2DAD_L);
    break;
  case 0xCE:
#ifdef dmawriteprint
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

	DM2DAD_H = value & 0x07FF;
    UPDATE_REG(0xCE, DM2DAD_H);
    break;
  case 0xD0:
#ifdef dmawriteprint

	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0


	DM2CNT_L = value & 0x3FFF;
    UPDATE_REG(0xD0, 0);
    break;
  case 0xD2:
#ifdef dmawriteprint

	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0


	  {
      bool start = ((DM2CNT_H ^ value) & 0x8000) ? true : false;
      
      value &= 0xF7E0;
      
      DM2CNT_H = value;
      UPDATE_REG(0xD2, DM2CNT_H);
      
      if(start && (value & 0x8000)) {
        dma2Source = DM2SAD_L | (DM2SAD_H << 16);
        dma2Dest = DM2DAD_L | (DM2DAD_H << 16);

        CPUCheckDMA(0, 4);
      }            
    }
    break;
  case 0xD4:
    DM3SAD_L = value;
    UPDATE_REG(0xD4, DM3SAD_L);
    break;
  case 0xD6:
    DM3SAD_H = value & 0x0FFF;
    UPDATE_REG(0xD6, DM3SAD_H);
    break;
  case 0xD8:
    DM3DAD_L = value;
    UPDATE_REG(0xD8, DM3DAD_L);
    break;
  case 0xDA:
    DM3DAD_H = value & 0x0FFF;
    UPDATE_REG(0xDA, DM3DAD_H);
    break;
  case 0xDC:
    DM3CNT_L = value;
    UPDATE_REG(0xDC, 0);
    break;
  case 0xDE:
    {
      bool start = ((DM3CNT_H ^ value) & 0x8000) ? true : false;

      value &= 0xFFE0;

      DM3CNT_H = value;
      UPDATE_REG(0xDE, DM3CNT_H);
    
      if(start && (value & 0x8000)) {
        dma3Source = DM3SAD_L | (DM3SAD_H << 16);
        dma3Dest = DM3DAD_L | (DM3DAD_H << 16);
        CPUCheckDMA(0,8);
      }
    }
    break;
 case 0x100:
    timer0Reload = value; //ichfly
#ifdef printsoundtimer
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0


	UPDATE_REG(0x100, value);
	*(u16 *)(0x4000100) = value;
    break;
  case 0x102:
    timer0Value = value;
    //timerOnOffDelay|=1;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x102, value);
#ifdef printsoundtimer
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0
	/*if(timer0Reload & 0x8000)
	{
		if((value & 0x3) == 0)
		{
			*(u16 *)(0x4000100) = timer0Reload >> 5;
			*(u16 *)(0x4000102) = value + 1;
			break;
		}
		if((value & 0x3) == 1)
		{
			*(u16 *)(0x4000100) = timer0Reload >> 1;
			*(u16 *)(0x4000102) = value + 1;
			break;
		}
		if((value & 3) == 2)
		{
			*(u16 *)(0x4000100) = timer0Reload >> 1;
			*(u16 *)(0x4000102) = value + 1;
			break;
		}
		*(u16 *)(0x4000102) = value;
		iprintf("big reload0\r\n");//todo 
	}
	else*/
	{	
		*(u16 *)(0x4000100) = timer1Reload << 1;
		*(u16 *)(0x4000102) = value;
	}
    break;
  case 0x104:
    timer1Reload = value;
#ifdef printsoundtimer
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

	UPDATE_REG(0x104, value);
	*(u16 *)(0x4000104) = value;
	break;
  case 0x106:
#ifdef printsoundtimer
	iprintf("ur %04x to %08x\r\n",value,address);
#endif
	REG_IPC_FIFO_TX = (address | 0x80000000);
	REG_IPC_FIFO_TX = (value | 0x80000000); //faster in case we send a 0

    timer1Value = value;
    //timerOnOffDelay|=2;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x106, value);

	/*if(timer1Reload & 0x8000)
	{
		if((value & 0x3) == 0)
		{
			*(u16 *)(0x4000104) = timer1Reload >> 5;
			*(u16 *)(0x4000106) = value + 1;
			break;
		}
		if((value & 0x3) == 1)
		{
			*(u16 *)(0x4000104) = timer1Reload >> 1;
			*(u16 *)(0x4000106) = value + 1;
			break;
		}
		if((value & 3) == 2)
		{
			*(u16 *)(0x4000104) = timer1Reload >> 1;
			*(u16 *)(0x4000106) = value + 1;
			break;
		}
		*(u16 *)(0x4000106) = value;
		iprintf("big reload1\r\n");//todo 
	}
	else*/
	{	
		*(u16 *)(0x4000104) = timer1Reload << 1;
		*(u16 *)(0x4000106) = value;
	}
	  break;
  case 0x108:
    timer2Reload = value;
	UPDATE_REG(0x108, value);
	*(u16 *)(0x4000108) = value;
    break;
  case 0x10A:
    timer2Value = value;
    //timerOnOffDelay|=4;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x10A, value);

	/*if(timer2Reload & 0x8000)
	{
		if((value & 0x3) == 0)
		{
			*(u16 *)(0x4000108) = timer2Reload >> 5;
			*(u16 *)(0x400010A) = value + 1;
			break;
		}
		if((value & 0x3) == 1)
		{
			*(u16 *)(0x4000108) = timer2Reload >> 1;
			*(u16 *)(0x400010A) = value + 1;
			break;
		}
		if((value & 3) == 2)
		{
			*(u16 *)(0x4000108) = timer2Reload >> 1;
			*(u16 *)(0x400010A) = value + 1;
			break;
		}
		iprintf("big reload2\r\n");//todo 
		*(u16 *)(0x400010A) = value;
	}
	else*/
	{	
		*(u16 *)(0x4000108) = timer2Reload << 1;
		*(u16 *)(0x400010A) = value;
	}
	  break;
  case 0x10C:
    timer3Reload = value;
	UPDATE_REG(0x10C, value);
    *(u16 *)(0x400010C) = value;
	  break;
  case 0x10E:
    timer3Value = value;
    //timerOnOffDelay|=8;
    //cpuNextEvent = cpuTotalTicks;
	UPDATE_REG(0x10E, value);

	/*if(timer3Reload & 0x8000)
	{
		if((value & 0x3) == 0)
		{
			*(u16 *)(0x400010C) = timer3Reload >> 5;
			*(u16 *)(0x400010E) = value + 1;
			break;
		}
		if((value & 0x3) == 1)
		{
			*(u16 *)(0x400010C) = timer3Reload >> 1;
			*(u16 *)(0x400010E) = value + 1;
			break;
		}
		if((value & 3) == 2)
		{
			*(u16 *)(0x400010C) = timer3Reload >> 1;
			*(u16 *)(0x400010E) = value + 1;
			break;
		}
		iprintf("big reload3\r\n");//todo 
		*(u16 *)(0x400010E) = value;
	}
	else*/
	{	
		*(u16 *)(0x400010C) = timer3Reload << 1;
		*(u16 *)(0x400010E) = value;
	}
  break;
  case 0x128:
    if(value & 0x80) {
      value &= 0xff7f;
      if(value & 1 && (value & 0x4000)) {
        UPDATE_REG(0x12a, 0xFF);
        IF |= 0x80;
        UPDATE_REG(0x202, IF);
        value &= 0x7f7f;
      }
    }
    UPDATE_REG(0x128, value);
    break;
  case 0x130:
    //P1 |= (value & 0x3FF); //ichfly readonly
    //UPDATE_REG(0x130, P1);
    break;
  case 0x132:
    UPDATE_REG(0x132, value & 0xC3FF);
	*(u16 *)(0x4000132) = value;
    break;
  case 0x200:
    IE = value & 0x3FFF;
    UPDATE_REG(0x200, IE);
    /*if ((IME & 1) && (IF & IE) && armIrqEnable)
      cpuNextEvent = cpuTotalTicks;*/
	  
	REG_IE = 1 | IE | (REG_IE & 0xFFFF0000); //todo filter the 1
	
	anytimejmpfilter = IE;
	
    break;
  case 0x202:
	REG_IF = value;
	if(value & 1)IF_VBl = 0;
	//IF = REG_IF;
    //IF ^= (value & IF);
    //UPDATE_REG(0x202, IF); //ichfly update at read
    break;
  case 0x204:
    { //ichfly can't emulate that
      /*memoryWait[0x0e] = memoryWaitSeq[0x0e] = gamepakRamWaitState[value & 3];
      
      if(!speedHack) {
        memoryWait[0x08] = memoryWait[0x09] = gamepakWaitState[(value >> 2) & 3];
        memoryWaitSeq[0x08] = memoryWaitSeq[0x09] =
          gamepakWaitState0[(value >> 4) & 1];
        
        memoryWait[0x0a] = memoryWait[0x0b] = gamepakWaitState[(value >> 5) & 3];
        memoryWaitSeq[0x0a] = memoryWaitSeq[0x0b] =
          gamepakWaitState1[(value >> 7) & 1];
        
        memoryWait[0x0c] = memoryWait[0x0d] = gamepakWaitState[(value >> 8) & 3];
        memoryWaitSeq[0x0c] = memoryWaitSeq[0x0d] =
          gamepakWaitState2[(value >> 10) & 1];
      } else {
        memoryWait[0x08] = memoryWait[0x09] = 3;
        memoryWaitSeq[0x08] = memoryWaitSeq[0x09] = 1;
        
        memoryWait[0x0a] = memoryWait[0x0b] = 3;
        memoryWaitSeq[0x0a] = memoryWaitSeq[0x0b] = 1;
        
        memoryWait[0x0c] = memoryWait[0x0d] = 3;
        memoryWaitSeq[0x0c] = memoryWaitSeq[0x0d] = 1;
      }
         
      for(int i = 8; i < 15; i++) {
        memoryWait32[i] = memoryWait[i] + memoryWaitSeq[i] + 1;
        memoryWaitSeq32[i] = memoryWaitSeq[i]*2 + 1;
      }

      if((value & 0x4000) == 0x4000) {
        busPrefetchEnable = true;
        busPrefetch = false;
        busPrefetchCount = 0;
      } else {
        busPrefetchEnable = false;
        busPrefetch = false;
        busPrefetchCount = 0;
      }*/
      UPDATE_REG(0x204, value & 0x7FFF);

    }
    break;
  case 0x208:
    IME = value & 1;
    UPDATE_REG(0x208, IME);
    if ((IME & 1) && (IF & IE) && armIrqEnable)
      cpuNextEvent = cpuTotalTicks;
    break;
  case 0x300:
    if(value != 0)
      value &= 0xFFFE;
    UPDATE_REG(0x300, value);
    break;
  default:
    UPDATE_REG(address&0x3FE, value);
    break;
  }
}

void CPUWriteHalfWord(u32 address, u16 value)
{
#ifdef printreads
iprintf("w16 %04x to %08x\r\n",value,address);
#endif

#ifdef DEV_VERSION
  if(address & 1) {
    if(systemVerbose & VERBOSE_UNALIGNED_MEMORY) {
      Log("Unaligned halfword write: %04x to %08x from %08x\n",
          value,
          address,
          armMode ? armNextPC - 4 : armNextPC - 2);
    }
  }
#endif
  
  switch(address >> 24) {
  case 2:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeWorkRAM[address & 0x3FFFE]))
      cheatsWriteHalfWord(address & 0x203FFFE,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unwritable;
#endif
      WRITE16LE(((u16 *)&workRAM[address & 0x3FFFE]),value);
    break;
  case 3:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeInternalRAM[address & 0x7ffe]))
      cheatsWriteHalfWord(address & 0x3007ffe,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unwritable;
#endif
      WRITE16LE(((u16 *)&internalRAM[address & 0x7ffe]), value);
    break;    
  case 4:
  
	/*if(address > 0x40000FF && address < 0x4000110)
	{
		*(u16 *)(address) = value;
		break;
	}*/ //don't need that
  
  	/*if(0x4000060 > address && address > 0x4000008)
	{
			iprintf("16 %x %x\r\n",address,value);
		    *(u16 *)((address & 0x3FF) + 0x4000000) = value;
	}*/ //dont do dobble
    if(address < 0x4000400)
      CPUUpdateRegister(address & 0x3fe, value);
    else goto unwritable;
    break;
  case 5:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezePRAM[address & 0x03fe]))
      cheatsWriteHalfWord(address & 0x70003fe,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unwritable;
#endif
    WRITE16LE(((u16 *)&paletteRAM[address & 0x3fe]), value);
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unwritable;
#endif
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeVRAM[address]))
      cheatsWriteHalfWord(address + 0x06000000,
                          value);
    else
#endif
    WRITE16LE(((u16 *)&vram[address]), value); 
    break;
  case 7:
#ifdef BKPT_SUPPORT
    if(*((u16 *)&freezeOAM[address & 0x03fe]))
      cheatsWriteHalfWord(address & 0x70003fe,
                          value);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x07000400)goto unwritable;
#endif
    WRITE16LE(((u16 *)&oam[address & 0x3fe]), value);
    break;
  case 8:
  case 9:
    if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8) {
      if(!rtcWrite(address, value))
        goto unwritable;
    } else if(!agbPrintWrite(address, value)) goto unwritable;
    break;
  case 13:
    if(cpuEEPROMEnabled) {
      eepromWrite(address, (u8)value);
      break;
    }
    goto unwritable;
  case 14:
    if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) {
      (*cpuSaveGameFunc)(address, (u8)value);
      break;
    }
    goto unwritable;
  default:
  unwritable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  Log("Illegal hword write: %04x to %08x\n",value, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

void CPUWriteByte(u32 address, u8 b)
{
#ifdef printreads
	iprintf("w8 %02x to %08x\r\n",b,address);
#endif
  switch(address >> 24) {
  case 2:
#ifdef BKPT_SUPPORT
      if(freezeWorkRAM[address & 0x3FFFF])
        cheatsWriteByte(address & 0x203FFFF, b);
      else
#endif
#ifdef checkclearaddrrw
	if(address >0x023FFFFF)goto unwritable;
#endif
        workRAM[address & 0x3FFFF] = b;
    break;
  case 3:
#ifdef BKPT_SUPPORT
    if(freezeInternalRAM[address & 0x7fff])
      cheatsWriteByte(address & 0x3007fff, b);
    else
#endif
#ifdef checkclearaddrrw
	if(address > 0x03008000 && !(address > 0x03FF8000)/*upern mirrow*/)goto unwritable;
#endif
      internalRAM[address & 0x7fff] = b;
    break;
  case 4:
  
    if(address < 0x4000400) {
      switch(address & 0x3FF) {
      case 0x301:
	if(b == 0x80)
	  stopState = true;
	holdState = 1;
	holdType = -1;
  cpuNextEvent = cpuTotalTicks;
	break;
      case 0x60:
      case 0x61:
      case 0x62:
      case 0x63:
      case 0x64:
      case 0x65:
      case 0x68:
      case 0x69:
      case 0x6c:
      case 0x6d:
      case 0x70:
      case 0x71:
      case 0x72:
      case 0x73:
      case 0x74:
      case 0x75:
      case 0x78:
      case 0x79:
      case 0x7c:
      case 0x7d:
      case 0x80:
      case 0x81:
      case 0x84:
      case 0x85:
      case 0x90:
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97:
      case 0x98:
      case 0x99:
      case 0x9a:
      case 0x9b:
      case 0x9c:
      case 0x9d:
      case 0x9e:
      case 0x9f:      
	//soundEvent(address&0xFF, b);  //ichfly disable sound
#ifdef printsoundwrites
		  iprintf("b %02x to %08x\r\n",b,address);
#endif
		  REG_IPC_FIFO_TX = (address | 0x40000000);
		  REG_IPC_FIFO_TX = (b | 0x80000000); //faster in case we send a 0
	break;
      default:
	/*if((0x4000060 > address && address > 0x4000008) || (address > 0x40000FF && address < 0x4000110))
	{
			//iprintf("8 %x %x\r\n",address,b);
		    *(u8 *)(address) = b;
	}*/ //ichfly don't need that
	if(address & 1)
	{
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16 *)&ioMem[address & 0x3fe])))
			     & 0x00FF) |
			    b<<8);

	}
	else
	  CPUUpdateRegister(address & 0x3fe,
			    ((READ16LE(((u16 *)&ioMem[address & 0x3fe])) & 0xFF00) | b));
      }
      break;
    } else goto unwritable;
    break;
  case 5:
#ifdef checkclearaddrrw
	if(address > 0x05000400)goto unwritable;
#endif
    // no need to switch
    *((u16 *)&paletteRAM[address & 0x3FE]) = (b << 8) | b;
    break;
  case 6:
#ifdef checkclearaddrrw
	if(address > 0x06020000)goto unwritable;
#endif
    address = (address & 0x1fffe);
    if (((DISPCNT & 7) >2) && ((address & 0x1C000) == 0x18000))
        return;
    if ((address & 0x18000) == 0x18000)
      address &= 0x17fff;

    // no need to switch 
    // byte writes to OBJ VRAM are ignored
    if ((address) < objTilesAddress[((DISPCNT&7)+1)>>2])
    {
#ifdef BKPT_SUPPORT
      if(freezeVRAM[address])
        cheatsWriteByte(address + 0x06000000, b);
      else
#endif  
            *((u16 *)&vram[address]) = (b << 8) | b;
    }
    break;
  case 7:
#ifdef checkclearaddrrw
	goto unwritable;
#endif
    // no need to switch
    // byte writes to OAM are ignored
    //    *((u16 *)&oam[address & 0x3FE]) = (b << 8) | b;
    break;    
  case 13:
    if(cpuEEPROMEnabled) {
      eepromWrite(address, b);
      break;
    }
    goto unwritable;
  case 14:
      if (!(saveType == 5) && (!eepromInUse | cpuSramEnabled | cpuFlashEnabled)) {

    //if(!cpuEEPROMEnabled && (cpuSramEnabled | cpuFlashEnabled)) { 

        (*cpuSaveGameFunc)(address, b);
      break;
    }
    // default
  default:
  unwritable:
#ifdef checkclearaddrrw
      //Log("Illegal word read: %08x at %08x\n", address,reg[15].I);
	  Log("Illegal byte write: %02x to %08x\n",b, address);
	  REG_IME = IME_DISABLE;
	  debugDump();
	  while(1);
#endif
    break;
  }
}

u8 cpuBitsSet[256];
u8 cpuLowestBitSet[256];

void CPUInit(const char *biosFileName, bool useBiosFile,bool extram)
{
#ifdef WORDS_BIGENDIAN
  if(!cpuBiosSwapped) {
    for(unsigned int i = 0; i < sizeof(myROM)/4; i++) {
      WRITE32LE(&myROM[i], myROM[i]);
    }
    cpuBiosSwapped = true;
  }
#endif
  gbaSaveType = 0;
  eepromInUse = 0;
  saveType = 0;
  useBios = false;
  
  if(useBiosFile) {
    int size = 0x4000;
    if(utilLoad(biosFileName,
                CPUIsGBABios,
                bios,
                size,extram)) {
      if(size == 0x4000)
        useBios = true;
      else
        systemMessage(MSG_INVALID_BIOS_FILE_SIZE, N_("Invalid BIOS file size"));
    }
  }
  
  if(!useBios) {
    memcpy(bios, myROM, sizeof(myROM));
  }

  int i = 0;

  biosProtected[0] = 0x00;
  biosProtected[1] = 0xf0;
  biosProtected[2] = 0x29;
  biosProtected[3] = 0xe1;

  for(i = 0; i < 256; i++) {
    int count = 0;
    int j;
    for(j = 0; j < 8; j++)
      if(i & (1 << j))
        count++;
    cpuBitsSet[i] = count;
    
    for(j = 0; j < 8; j++)
      if(i & (1 << j))
        break;
    cpuLowestBitSet[i] = j;
  }

  for(i = 0; i < 0x400; i++)
    ioReadable[i] = true;
  for(i = 0x10; i < 0x48; i++)
    ioReadable[i] = false;
  for(i = 0x4c; i < 0x50; i++)
    ioReadable[i] = false;
  for(i = 0x54; i < 0x60; i++)
    ioReadable[i] = false;
  for(i = 0x8c; i < 0x90; i++)
    ioReadable[i] = false;
  for(i = 0xa0; i < 0xb8; i++)
    ioReadable[i] = false;
  for(i = 0xbc; i < 0xc4; i++)
    ioReadable[i] = false;
  for(i = 0xc8; i < 0xd0; i++)
    ioReadable[i] = false;
  for(i = 0xd4; i < 0xdc; i++)
    ioReadable[i] = false;
  for(i = 0xe0; i < 0x100; i++)
    ioReadable[i] = false;
  for(i = 0x110; i < 0x120; i++)
    ioReadable[i] = false;
  for(i = 0x12c; i < 0x130; i++)
    ioReadable[i] = false;
  for(i = 0x138; i < 0x140; i++)
    ioReadable[i] = false;
  for(i = 0x144; i < 0x150; i++)
    ioReadable[i] = false;
  for(i = 0x15c; i < 0x200; i++)
    ioReadable[i] = false;
  for(i = 0x20c; i < 0x300; i++)
    ioReadable[i] = false;
  for(i = 0x304; i < 0x400; i++)
    ioReadable[i] = false;

  if(romSize < 0x1fe2000) {
    *((u16 *)&rom[0x1fe209c]) = 0xdffa; // SWI 0xFA
    *((u16 *)&rom[0x1fe209e]) = 0x4770; // BX LR
  } else {
    agbPrintEnable(false);
  }
}

void CPUReset()
{


  if(gbaSaveType == 0) {
    if(eepromInUse)
      gbaSaveType = 3;
    else
      switch(saveType) {
      case 1:
        gbaSaveType = 1;
        break;
      case 2:
        gbaSaveType = 2;
        break;
      }
  }
  rtcReset();


  // clean registers
  memset(&reg[0], 0, sizeof(reg));
  // clean OAM
  //memset(oam, 0, 0x400);
  // clean palette
  //memset(paletteRAM, 0, 0x400);
  // clean picture
  //memset(pix, 0, 4*160*240);
  // clean vram
  //memset(vram, 0, 0x20000);
  // clean io memory
  memset(ioMem, 0, 0x400);





  //DISPCNT  = 0x0000;
  DISPSTAT = 0x0000;
  VCOUNT   = (useBios && !skipBios) ? 0 :0x007E;
  BG0CNT   = 0x0000;
  BG1CNT   = 0x0000;
  BG2CNT   = 0x0000;
  BG3CNT   = 0x0000;
  BG0HOFS  = 0x0000;
  BG0VOFS  = 0x0000;
  BG1HOFS  = 0x0000;
  BG1VOFS  = 0x0000;
  BG2HOFS  = 0x0000;
  BG2VOFS  = 0x0000;
  BG3HOFS  = 0x0000;
  BG3VOFS  = 0x0000;
  BG2PA    = 0x0100;
  BG2PB    = 0x0000;
  BG2PC    = 0x0000;
  BG2PD    = 0x0100;
  BG2X_L   = 0x0000;
  BG2X_H   = 0x0000;
  BG2Y_L   = 0x0000;
  BG2Y_H   = 0x0000;
  BG3PA    = 0x0100;
  BG3PB    = 0x0000;
  BG3PC    = 0x0000;
  BG3PD    = 0x0100;
  BG3X_L   = 0x0000;
  BG3X_H   = 0x0000;
  BG3Y_L   = 0x0000;
  BG3Y_H   = 0x0000;
  WIN0H    = 0x0000;
  WIN1H    = 0x0000;
  WIN0V    = 0x0000;
  WIN1V    = 0x0000;
  WININ    = 0x0000;
  WINOUT   = 0x0000;
  MOSAIC   = 0x0000;
  BLDMOD   = 0x0000;
  COLEV    = 0x0000;
  COLY     = 0x0000;
  DM0SAD_L = 0x0000;
  DM0SAD_H = 0x0000;
  DM0DAD_L = 0x0000;
  DM0DAD_H = 0x0000;
  DM0CNT_L = 0x0000;
  DM0CNT_H = 0x0000;
  DM1SAD_L = 0x0000;
  DM1SAD_H = 0x0000;
  DM1DAD_L = 0x0000;
  DM1DAD_H = 0x0000;
  DM1CNT_L = 0x0000;
  DM1CNT_H = 0x0000;
  DM2SAD_L = 0x0000;
  DM2SAD_H = 0x0000;
  DM2DAD_L = 0x0000;
  DM2DAD_H = 0x0000;
  DM2CNT_L = 0x0000;
  DM2CNT_H = 0x0000;
  DM3SAD_L = 0x0000;
  DM3SAD_H = 0x0000;
  DM3DAD_L = 0x0000;
  DM3DAD_H = 0x0000;
  DM3CNT_L = 0x0000;
  DM3CNT_H = 0x0000;
  TM0D     = 0x0000;
  TM0CNT   = 0x0000;
  TM1D     = 0x0000;
  TM1CNT   = 0x0000;
  TM2D     = 0x0000;
  TM2CNT   = 0x0000;
  TM3D     = 0x0000;
  TM3CNT   = 0x0000;
  P1       = 0x03FF;
  IE       = 0x0000;
  IF       = 0x0000;
  IME      = 0x0000;

  armMode = 0x1F;
  
  if(cpuIsMultiBoot) {
    reg[13].I = 0x03007F00;
    reg[15].I = 0x02000000;
    reg[16].I = 0x00000000;
    reg[R13_IRQ].I = 0x03007FA0;
    reg[R13_SVC].I = 0x03007FE0;
    armIrqEnable = true;
  } else {
    if(useBios && !skipBios) {
      reg[15].I = 0x00000000;
      armMode = 0x13;
      armIrqEnable = false;  
    } else {
      reg[13].I = 0x03007F00;
      reg[15].I = 0x08000000;
      reg[16].I = 0x00000000;
      reg[R13_IRQ].I = 0x03007FA0;
      reg[R13_SVC].I = 0x03007FE0;
      armIrqEnable = true;      
    }    
  }
  armState = true;
  C_FLAG = V_FLAG = N_FLAG = Z_FLAG = false;
  UPDATE_REG(0x00, DISPCNT);
  UPDATE_REG(0x06, VCOUNT);
  UPDATE_REG(0x20, BG2PA);
  UPDATE_REG(0x26, BG2PD);
  UPDATE_REG(0x30, BG3PA);
  UPDATE_REG(0x36, BG3PD);
  UPDATE_REG(0x130, P1);
  UPDATE_REG(0x88, 0x200);

  // disable FIQ
  reg[16].I |= 0x40;
  
  CPUUpdateCPSR();
  
  armNextPC = reg[15].I;
  reg[15].I += 4;

  // reset internal state
  holdState = false;
  holdType = 0;
  
  biosProtected[0] = 0x00;
  biosProtected[1] = 0xf0;
  biosProtected[2] = 0x29;
  biosProtected[3] = 0xe1;
  
  lcdTicks = (useBios && !skipBios) ? 1008 : 208;
  timer0On = false;
  timer0Ticks = 0;
  timer0Reload = 0;
  timer0ClockReload  = 0;
  timer1On = false;
  timer1Ticks = 0;
  timer1Reload = 0;
  timer1ClockReload  = 0;
  timer2On = false;
  timer2Ticks = 0;
  timer2Reload = 0;
  timer2ClockReload  = 0;
  timer3On = false;
  timer3Ticks = 0;
  timer3Reload = 0;
  timer3ClockReload  = 0;
  dma0Source = 0;
  dma0Dest = 0;
  dma1Source = 0;
  dma1Dest = 0;
  dma2Source = 0;
  dma2Dest = 0;
  dma3Source = 0;
  dma3Dest = 0;
  cpuSaveGameFunc = flashSaveDecide;
  //renderLine = mode0RenderLine;
  fxOn = false;
  windowOn = false;
  frameCount = 0;
  saveType = 0;
  //layerEnable = DISPCNT & layerSettings;

  //CPUUpdateRenderBuffers(true);
  
  for(int i = 0; i < 256; i++) {
    map[i].address = (u8 *)&dummyAddress;
    map[i].mask = 0;
  }




  map[0].address = bios;
  map[0].mask = 0x3FFF;
  map[2].address = workRAM;
  map[2].mask = 0x3FFFF;
  map[3].address = internalRAM;
  map[3].mask = 0x7FFF;
  map[4].address = ioMem;
  map[4].mask = 0x3FF;
  map[5].address = paletteRAM;
  map[5].mask = 0x3FF;
  map[6].address = vram;
  map[6].mask = 0x1FFFF;
  map[7].address = oam;
  map[7].mask = 0x3FF;
  map[8].address = rom;
  map[8].mask = 0x1FFFFFF;
  map[9].address = rom;
  map[9].mask = 0x1FFFFFF;  
  map[10].address = rom;
  map[10].mask = 0x1FFFFFF;
  map[12].address = rom;
  map[12].mask = 0x1FFFFFF;
  map[14].address = flashSaveMemory;
  map[14].mask = 0xFFFF;



  eepromReset();
  flashReset();
  
  //soundReset(); //ichfly sound

  //CPUUpdateWindow0();
  //CPUUpdateWindow1();

  // make sure registers are correctly initialized if not using BIOS
  /*if(!useBios) {
    if(cpuIsMultiBoot)
      BIOS_RegisterRamReset(0xfe);
    else
      BIOS_RegisterRamReset(0xff);
  } else {
    if(cpuIsMultiBoot)
      BIOS_RegisterRamReset(0xfe);
  }*/ //ararar

  switch(cpuSaveType) {
  case 0: // automatic
    cpuSramEnabled = true;
    cpuFlashEnabled = true;
    cpuEEPROMEnabled = true;
    cpuEEPROMSensorEnabled = false;
    saveType = gbaSaveType = 0;
    break;
  case 1: // EEPROM
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = true;
    cpuEEPROMSensorEnabled = false;
    saveType = gbaSaveType = 3;
    // EEPROM usage is automatically detected
    break;
  case 2: // SRAM
    cpuSramEnabled = true;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    cpuSaveGameFunc = sramDelayedWrite; // to insure we detect the write
    saveType = gbaSaveType = 1;
    break;
  case 3: // FLASH
    cpuSramEnabled = false;
    cpuFlashEnabled = true;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    cpuSaveGameFunc = flashDelayedWrite; // to insure we detect the write
    saveType = gbaSaveType = 2;
    break;
  case 4: // EEPROM+Sensor
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = true;
    cpuEEPROMSensorEnabled = true;
    // EEPROM usage is automatically detected
    saveType = gbaSaveType = 3;
    break;
  case 5: // NONE
    cpuSramEnabled = false;
    cpuFlashEnabled = false;
    cpuEEPROMEnabled = false;
    cpuEEPROMSensorEnabled = false;
    // no save at all
    saveType = gbaSaveType = 5;
    break;
  } 

  ARM_PREFETCH;

  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

  cpuDmaHack = false;

  lastTime = systemGetClock();

  SWITicks = 0;
}

#ifdef SDL
void Log(const char *defaultMsg, ...)
{
  char buffer[2048];
  va_list valist;
  
  va_start(valist, defaultMsg);
  vsprintf(buffer, defaultMsg, valist);

  if(out == NULL) {
    out = fopen("trace.Log","w");
  }

  fputs(buffer, out);
  
  va_end(valist);
}
#else
extern void winLog(const char *, ...);
#endif




struct EmulatedSystem GBASystem = {
  // emuMain
  //CPULoop,
  // emuReset
  CPUReset,
  // emuCleanUp
  CPUCleanUp,
  // emuReadBattery
  CPUReadBatteryFile,
  // emuWriteBattery
  CPUWriteBatteryFile,
  // emuReadState
  CPUReadState,
  // emuWriteState 
  CPUWriteState,
  // emuReadMemState
  CPUReadMemState,
  // emuWriteMemState
  CPUWriteMemState,
  // emuWritePNG
  CPUWritePNGFile,
  // emuWriteBMP
  CPUWriteBMPFile,
  // emuUpdateCPSR
  CPUUpdateCPSR,
  // emuHasDebugger
  true,
  // .
  2500000
};

//ichfly

u32 systemGetClock()
{
  return 0; //ichfly todo ftp counter 
}

void winLog(char const*, ...)
{
	return; //todo ichfly
}

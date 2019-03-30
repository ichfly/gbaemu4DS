// -*- C++ -*-
// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

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

#ifndef VBA_SYSTEM_H
#define VBA_SYSTEM_H

#include <stdio.h> //ichfly
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

#ifndef NULL
#define NULL 0
#endif

//extern void log(const char *,...);

#define SYSTEM_SAVE_UPDATED 30
#define SYSTEM_SAVE_NOT_UPDATED 0

#endif //VBA_SYSTEM_H


//MPU NDS Definitions
#ifdef __cplusplus
extern "C" {
#endif

//System.cpp
extern bool systemPauseOnFrame();
extern void systemGbPrint(u8 * a,int,int,int,int);
extern void systemScreenCapture(int);
extern void systemDrawScreen();
// updates the joystick data
extern bool systemReadJoypads();
// return information about the given joystick, -1 for default joystick
extern u32 systemReadJoypad(int);
extern u32 systemGetClock();
extern void systemMessage(int, const char *, ...);
extern void systemSetTitle(const char *);
extern void systemWriteDataToSoundBuffer();
extern void systemSoundShutdown();
extern void systemSoundPause();
extern void systemSoundResume();
extern void systemSoundReset();
extern bool systemSoundInit();
extern void systemScreenMessage(const char *);
extern void systemUpdateMotionSensor();
extern int  systemGetSensorX();
extern int  systemGetSensorY();
extern bool systemCanChangeSoundQuality();
extern void systemShowSpeed(int);
extern void system10Frames(int);
extern void systemFrame();
extern void systemGbBorderOn();

extern void Sm60FPS_Init();
extern bool Sm60FPS_CanSkipFrame();
extern void Sm60FPS_Sleep();
extern void DbgMsg(const char *msg, ...);
extern void winlog(const char *,...);

extern bool systemSoundOn;
extern u16 systemGbPalette[24];
extern int systemRedShift;
extern int systemGreenShift;
extern int systemBlueShift;
extern int systemColorDepth;
extern int systemDebug;
extern int systemVerbose;
extern int systemFrameSkip;
extern int systemSaveUpdateCounter;
extern int systemSpeed;

extern int len;

//icache.s | dcache.s

// extern void puSetMemPerm(u32 perm);
extern void pu_Enable();

// extern void puSetGbaIWRAM();
extern void pu_SetRegion(u32 region, u32 value);

extern void pu_SetDataPermissions(u32 v);
extern void pu_SetCodePermissions(u32 v);
extern void pu_SetDataCachability(u32 v);
extern void pu_SetCodeCachability(u32 v);
extern void pu_GetWriteBufferability(u32 v);

extern void cpu_SetCP15Cnt(u32 v); //mask bit 1 for: 0 disable, 1 enable, PU
extern u32 cpu_GetCP15Cnt(); //get PU status: 0 disable, 1 enable

//instruction cache CP15
extern void IC_InvalidateAll();
extern void IC_InvalidateRange(const void *, u32 v);

//data cache CP15
extern void DC_FlushAll();
extern void DC_FlushRange(const void *, u32 v);

//ex_s.s
extern u32 savedsp;
extern u32 savedlr;
extern void gbaExceptionHdl();
extern int SPtoload;
extern int SPtemp;

#ifdef __cplusplus
}
#endif
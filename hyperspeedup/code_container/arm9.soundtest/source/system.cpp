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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>


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

//#include <SDL.h> //ichfly realy todo
//#include <SDL_thread.h>


#define __DOUTBUFSIZE 256

char __outstr[__DOUTBUFSIZE];

/*#include "../GBA.h"
#include "../gb/GB.h"
#include "../gb/gbGlobals.h"
#include "../Util.h"
#include "../Sound.h"*/

//#include "window.h"
//#include "intl.h"

// Required vars, used by the emulator core
//
int  systemRedShift;
int  systemGreenShift;
int  systemBlueShift;
int  systemColorDepth;
int  systemDebug;
int  systemVerbose;
int  systemSaveUpdateCounter;
int  systemFrameSkip;
bool systemSoundOn;

int  emulating;
bool debugger;
int  RGB_LOW_BITS_MASK;

// Extra vars, only used for the GUI
//
int systemRenderedFrames;
int systemFPS;

// Sound stuff
//
/*const  int         iSoundSamples  = 2048;
const  int         iSoundTotalLen = iSoundSamples * 4;
static u8          auiSoundBuffer[iSoundTotalLen];
static int         iSoundLen;
static SDL_cond *  pstSoundCond;
static SDL_mutex * pstSoundMutex;

inline VBA::Window * GUI()
{
  return VBA::Window::poGetInstance();
}
*/
void systemMessage(int _iId, const char * _csFormat, ...)
{
	va_list args;
	int len;

	va_start(args, _csFormat);
	len=vsnprintf(__outstr,__DOUTBUFSIZE,_csFormat,args);
	va_end(args);

  iprintf(__outstr);//GUI()->vPopupErrorV(_(_csFormat), args);

  va_end(args);
  
  
}
void systemUpdateMotionSensor()
{
}

int systemGetSensorX()
{
  return 0;
}

int systemGetSensorY()
{
  return 0;
}

void debuggerOutput(char *, u32)
{
}

void (*dbgOutput)(char *, u32) = debuggerOutput;

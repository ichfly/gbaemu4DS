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
#include <string.h>

#include "GBA.h"
#include "Globals.h"
#include "Port.h"

#define debuggerWriteHalfWord(addr, value) \
  WRITE16LE((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask], (value))

//ichfly newmap
/*#define debuggerReadHalfWord(addr) \
  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))*/
u16 debuggerReadHalfWord(u16 addr) //ichfly here rom reader
{
	if(map[((addr)>>mapseekoffs) & mapander].status == 0) // 0 = full loaded
	{
		return READ16LE((u16*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
	}
	else
	{
		if(map[((addr)>>mapseekoffs) & mapander].status == 1) //1 = load in progress
		{
			while(map[((addr)>>mapseekoffs) & mapander].loaded + dmagetloaded() < addr + 2); //sorry you need to wait :(
			return READ16LE((u16*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
		}
		if(map[((addr)>>mapseekoffs) & mapander].status == 2) //2 = not loaded
		{
			dmastartloadin(addr);
			while(map[((addr)>>mapseekoffs) & mapander].loaded + dmagetloaded() < addr + 2); //sorry you need to wait :(
			return READ16LE((u16*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
		}
	}
}

static bool agbPrintEnabled = false;
static bool agbPrintProtect = false;

bool agbPrintWrite(u32 address, u16 value)
{
  if(agbPrintEnabled) {
    if(address == 0x9fe2ffe) { // protect
      agbPrintProtect = (value != 0);
      debuggerWriteHalfWord(address, value);
      return true;
    } else {
      if(agbPrintProtect &&
         ((address >= 0x9fe20f8 && address <= 0x9fe20ff) // control structure
          || (address >= 0x8fd0000 && address <= 0x8fdffff)
          || (address >= 0x9fd0000 && address <= 0x9fdffff))) {
        debuggerWriteHalfWord(address, value);
        return true;
      }
    }
  }
  return false;
}

void agbPrintReset()
{
  agbPrintProtect = false;
}

void agbPrintEnable(bool enable)
{
  agbPrintEnabled = enable;
}

bool agbPrintIsEnabled()
{
  return agbPrintEnabled;
}

extern void (*dbgOutput)(char *, u32);
 
void agbPrintFlush()
{
  u16 get = debuggerReadHalfWord(0x9fe20fc);
  u16 put = debuggerReadHalfWord(0x9fe20fe);

  u32 address = (debuggerReadHalfWord(0x9fe20fa) << 16);
  if(address != 0xfd0000 && address != 0x1fd0000) {
    dbgOutput("Did you forget to call AGBPrintInit?\n", 0);
    // get rid of the text otherwise we will continue to be called
    debuggerWriteHalfWord(0x9fe20fc, put);    
    return;
  }

  u8 *data = &rom[address]; //ichfly here rom reader todo not working

  while(get != put) {
    char c = data[get++];
    char s[2];
    s[0] = c;
    s[1] = 0;

    if(systemVerbose & VERBOSE_AGBPRINT)
      dbgOutput(s, 0);
    if(c == '\n')
      break;
  }
  debuggerWriteHalfWord(0x9fe20fc, get);
}

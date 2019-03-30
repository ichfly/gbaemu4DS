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

#define voidp void*

#include <nds.h>
#include <stdio.h>

#ifndef VBA_UTIL_H
#define VBA_UTIL_H
enum IMAGE_TYPE {
  IMAGE_UNKNOWN = -1,
  IMAGE_GBA     = 0,
  IMAGE_GB      = 1
};

#endif


#ifdef __cplusplus
extern "C" {
#endif

void generatefilemap(int size);

extern bool utilWritePNGFile(const char *, int, int, u8 *);
extern bool utilWriteBMPFile(const char *, int, int, u8 *);
extern void utilApplyIPS(const char *ips, u8 **rom, int *size);
extern void utilWriteBMP(char *, int, int, u8 *);
extern bool utilIsGBAImage(const char *);
extern bool utilIsSAV(const char * file);
extern bool utilIsGBImage(const char *);
extern bool utilIsZipFile(const char *);
extern bool utilIsGzipFile(const char *);
extern bool utilIsRarFile(const char *);
extern void utilGetBaseName(const char *, char *);
//extern IMAGE_TYPE utilFindType(const char *);
extern u8 *utilLoad(const char *,
                    u8 *,
                    int &,bool);

extern void utilPutDword(u8 *, u32);
extern void utilPutWord(u8 *, u16);
extern void utilWriteData(gzFile, variable_desc *);
extern void utilReadData(gzFile, variable_desc *);
extern int utilReadInt(gzFile);
extern void utilWriteInt(gzFile, int);
extern gzFile utilGzOpen(const char *file, const char *mode);
extern gzFile utilMemGzOpen(char *memory, int available, char *mode);
extern int utilGzWrite(gzFile file, const voidp buffer, unsigned int len);
extern int utilGzRead(gzFile file, voidp buffer, unsigned int len);
extern int utilGzClose(gzFile file);
extern long utilGzMemTell(gzFile file);
extern void utilGBAFindSave(const u8 *, const int);
extern void utilUpdateSystemColorMaps();
extern int anytimejmpfilter;
extern int utilGetSize(int size);

extern void UPDATE_REG(u16 address, u16 value);
extern void WRITE32LE(u32 * x, u32 v);
extern void WRITE16LE(u16 * x, u16 v);
extern u32 READ32LE(u32 * x);
extern u16 READ16LE(u16 * x);

extern u8 CPUReadByteQuick(u32 addr);
extern u16 CPUReadHalfWordQuick(u32 addr);
extern u32 CPUReadMemoryQuick(u32 addr);

extern u16 swap16(u16 v);
extern u32 swap32(u32 v);

extern int save_decider(bool overrideSaveSettings);
extern bool useMPUFast;

extern bool save_deciderByTitle(char * headerTitleSource, char * headerTitleHaystack, int SizeToCheck);
extern bool pendingSaveFix;
extern int  SaveSizeBeforeFix;
extern int  SaveSizeAfterFix;
extern char* strtoupper(char* s);
extern char* strtolower(char* s);

#ifdef __cplusplus
}
#endif

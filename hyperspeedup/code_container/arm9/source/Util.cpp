// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004-2006 Forgotten and the VBA development team

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
#include <stdlib.h>
#include <string.h>
//#include <zlib.h> //todo ichfly

#include <unistd.h>    // for sbrk()

#include <fat.h>

extern "C" {
//#include <png.h> //ichfly todo ganze datei!!!!!!!! realy todo
}
#include "ichflysettings.h"

#if 0
#include "unrarlib.h"
#endif

#include "System.h"
#include "NLS.h"
#include "Util.h"
#include "Flash.h"
#include "GBA.h"
#include "Globals.h"
#include "RTC.h"
#include "Port.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif // ! _MSC_VER

extern int systemColorDepth;
extern int systemRedShift;
extern int systemGreenShift;
extern int systemBlueShift;


/*
static int (ZEXPORT *utilGzWriteFunc)(gzFile, const voidp, unsigned int) = NULL;
static int (ZEXPORT *utilGzReadFunc)(gzFile, voidp, unsigned int) = NULL;
static int (ZEXPORT *utilGzCloseFunc)(gzFile) = NULL;

bool utilWritePNGFile(const char *fileName, int w, int h, u8 *pix)
{
  u8 writeBuffer[512 * 3];
  
  FILE *fp = fopen(fileName,"wb");

  if(!fp) {
    systemMessage(MSG_ERROR_CREATING_FILE, N_("Error creating file %s"), fileName);
    return false;
  }
  
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                NULL,
                                                NULL,
                                                NULL);
  if(!png_ptr) {
    fclose(fp);
    return false;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);

  if(!info_ptr) {
    png_destroy_write_struct(&png_ptr,NULL);
    fclose(fp);
    return false;
  }

  if(setjmp(png_ptr->jmpbuf)) {
    png_destroy_write_struct(&png_ptr,NULL);
    fclose(fp);
    return false;
  }

  png_init_io(png_ptr,fp);

  png_set_IHDR(png_ptr,
               info_ptr,
               w,
               h,
               8,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png_ptr,info_ptr);

  u8 *b = writeBuffer;

  int sizeX = w;
  int sizeY = h;

  switch(systemColorDepth) {
  case 16:
    {
      u16 *p = (u16 *)(pix+(w+2)*2); // skip first black line
      for(int y = 0; y < sizeY; y++) {
         for(int x = 0; x < sizeX; x++) {
          u16 v = *p++;
          
          *b++ = ((v >> systemRedShift) & 0x001f) << 3; // R
          *b++ = ((v >> systemGreenShift) & 0x001f) << 3; // G 
          *b++ = ((v >> systemBlueShift) & 0x01f) << 3; // B
        }
        p++; // skip black pixel for filters
        p++; // skip black pixel for filters
        png_write_row(png_ptr,writeBuffer);
        
        b = writeBuffer;
      }
    }
    break;
  case 24:
    {
      u8 *pixU8 = (u8 *)pix;
      for(int y = 0; y < sizeY; y++) {
        for(int x = 0; x < sizeX; x++) {
          if(systemRedShift < systemBlueShift) {
            *b++ = *pixU8++; // R
            *b++ = *pixU8++; // G
            *b++ = *pixU8++; // B
          } else {
            int blue = *pixU8++;
            int green = *pixU8++;
            int red = *pixU8++;
            
            *b++ = red;
            *b++ = green;
            *b++ = blue;
          }
        }
        png_write_row(png_ptr,writeBuffer);
        
        b = writeBuffer;
      }
    }
    break;
  case 32:
    {
      u32 *pixU32 = (u32 *)(pix+4*(w+1));
      for(int y = 0; y < sizeY; y++) {
        for(int x = 0; x < sizeX; x++) {
          u32 v = *pixU32++;
          
          *b++ = ((v >> systemRedShift) & 0x001f) << 3; // R
          *b++ = ((v >> systemGreenShift) & 0x001f) << 3; // G
          *b++ = ((v >> systemBlueShift) & 0x001f) << 3; // B
        }
        pixU32++;
        
        png_write_row(png_ptr,writeBuffer);
        
        b = writeBuffer;
      }
    }
    break;
  }
  
  png_write_end(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);

  return true;  
}
*/
extern bool cpuIsMultiBoot;


bool utilIsSAV(const char * file)
{
  if(strlen(file) > 4) {
    const char * p = strrchr(file,'.');

    if(p != NULL) {
      if(_stricmp(p, ".sav") == 0) {
        cpuIsMultiBoot = true;
        return true;
      }
    }
  }

  return false;
}



bool utilIsGBAImage(const char * file)
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
      /*if(_stricmp(p, ".elf") == 0)
        return true;*/ //todo
      if(_stricmp(p, ".mb") == 0) {
        cpuIsMultiBoot = true;
        return true;
      }
    }
  }

  return false;
}

/*
IMAGE_TYPE utilFindType(const char *file)
{
  char buffer[2048];
  
  if(utilIsZipFile(file)) {
    unzFile unz = unzOpen(file);
    
    if(unz == NULL) {
      systemMessage(MSG_CANNOT_OPEN_FILE, N_("Cannot open file %s"), file);
      return IMAGE_UNKNOWN;
    }
    
    int r = unzGoToFirstFile(unz);
    
    if(r != UNZ_OK) {
      unzClose(unz);
      systemMessage(MSG_BAD_ZIP_FILE, N_("Bad ZIP file %s"), file);
      return IMAGE_UNKNOWN;
    }
    
    IMAGE_TYPE found = IMAGE_UNKNOWN;
    
    unz_file_info info;
    
    while(true) {
      r = unzGetCurrentFileInfo(unz,
                                &info,
                                buffer,
                                sizeof(buffer),
                                NULL,
                                0,
                                NULL,
                                0);
      
      if(r != UNZ_OK) {
        unzClose(unz);
        systemMessage(MSG_BAD_ZIP_FILE, N_("Bad ZIP file %s"), file);
        return IMAGE_UNKNOWN;
      }
      
      if(utilIsGBAImage(buffer)) {
        found = IMAGE_GBA;
        break;
      }

      if(utilIsGBImage(buffer)) {
        found = IMAGE_GB;
        break;
      }
        
      r = unzGoToNextFile(unz);
      
      if(r != UNZ_OK)
        break;
    }
    unzClose(unz);
    
    if(found == IMAGE_UNKNOWN) {
      systemMessage(MSG_NO_IMAGE_ON_ZIP,
                    N_("No image found on ZIP file %s"), file);
      return found;
    }
    return found;
#if 0
  } else if(utilIsRarFile(file)) {
    IMAGE_TYPE found = IMAGE_UNKNOWN;
    
    ArchiveList_struct *rarList = NULL;
    if(urarlib_list((void *)file, (ArchiveList_struct *)&rarList)) {
      ArchiveList_struct *p = rarList;

      while(p) {
        if(utilIsGBAImage(p->item.Name)) {
          found = IMAGE_GBA;
          break;
        }

        if(utilIsGBImage(p->item.Name)) {
          found = IMAGE_GB;
          break;
        }
        p = p->next;
      }
      
      urarlib_freelist(rarList);
    }
    return found;
#endif
  } else {
    if(utilIsGzipFile(file))
      utilGetBaseName(file, buffer);
    else
      strcpy(buffer, file);
    
    if(utilIsGBAImage(buffer))
      return IMAGE_GBA;
    if(utilIsGBImage(buffer))
      return IMAGE_GB;
  }
  return IMAGE_UNKNOWN;  
}
*/
static int utilGetSize(int size)
{
  int res = 1;
  while(res < size)
    res <<= 1;
  return res;
}
/*
static u8 *utilLoadFromZip(const char *file,
                           bool (*accept)(const char *),
                           u8 *data,
                           int &size)
{
  char buffer[2048];
  
  unzFile unz = unzOpen(file);
    
  if(unz == NULL) {
    systemMessage(MSG_CANNOT_OPEN_FILE, N_("Cannot open file %s"), file);
    return NULL;
  }
  int r = unzGoToFirstFile(unz);
    
  if(r != UNZ_OK) {
    unzClose(unz);
    systemMessage(MSG_BAD_ZIP_FILE, N_("Bad ZIP file %s"), file);
    return NULL;
  }
    
  bool found = false;
    
  unz_file_info info;
  
  while(true) {
    r = unzGetCurrentFileInfo(unz,
                              &info,
                              buffer,
                              sizeof(buffer),
                              NULL,
                              0,
                              NULL,
                              0);
      
    if(r != UNZ_OK) {
      unzClose(unz);
      systemMessage(MSG_BAD_ZIP_FILE, N_("Bad ZIP file %s"), file);
      return NULL;
    }

    if(accept(buffer)) {
      found = true;
      break;
    }
    
    r = unzGoToNextFile(unz);
      
    if(r != UNZ_OK)
      break;
  }

  if(!found) {
    unzClose(unz);
    systemMessage(MSG_NO_IMAGE_ON_ZIP,
                  N_("No image found on ZIP file %s"), file);
    return NULL;
  }
  
  int fileSize = info.uncompressed_size;
  if(size == 0)
    size = fileSize;
  r = unzOpenCurrentFile(unz);

  if(r != UNZ_OK) {
    unzClose(unz);
    systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error opening image %s"), buffer);
    return NULL;
  }

  u8 *image = data;
  
  if(image == NULL) {
    image = (u8 *)malloc(utilGetSize(size));
    if(image == NULL) {
      unzCloseCurrentFile(unz);
      unzClose(unz);
      systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                    "data");
      return NULL;
    }
    size = fileSize;
  }
  int read = fileSize <= size ? fileSize : size;
  r = unzReadCurrentFile(unz,
                         image,
                         read);

  unzCloseCurrentFile(unz);
  unzClose(unz);
  
  if(r != (int)read) {
    systemMessage(MSG_ERROR_READING_IMAGE,
                  N_("Error reading image %s"), buffer);
    if(data == NULL)
      free(image);
    return NULL;
  }

  size = fileSize;

  return image;
}

static u8 *utilLoadGzipFile(const char *file,
                            bool (*accept)(const char *),
                            u8 *data,
                            int &size)
{
  FILE *f = fopen(file, "rb");

  if(f == NULL) {
    systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error opening image %s"), file);
    return NULL;
  }

  fseek(f, -4, SEEK_END);
  int fileSize = fgetc(f) | (fgetc(f) << 8) | (fgetc(f) << 16) | (fgetc(f) << 24);
  fclose(f);
  if(size == 0)
    size = fileSize;

  gzFile gz = gzopen(file, "rb");

  if(gz == NULL) {
    // should not happen, but who knows?
    systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error opening image %s"), file);
    return NULL;
  }

  u8 *image = data;

  if(image == NULL) {
    image = (u8 *)malloc(utilGetSize(size));
    if(image == NULL) {
      systemMessage(MSG_OUT_OF_MEMORY, N_("Failed to allocate memory for %s"),
                    "data");
      fclose(f);
      return NULL;
    }
    size = fileSize;
  }
  int read = fileSize <= size ? fileSize : size;
  int r = gzread(gz, image, read);
  gzclose(gz);

  if(r != (int)read) {
    systemMessage(MSG_ERROR_READING_IMAGE,
                  N_("Error reading image %s"), file);
    if(data == NULL)
      free(image);
    return NULL;
  }  
  
  size = fileSize;

  return image;  
}
*/
#if 0
static u8 *utilLoadRarFile(const char *file,
                           bool (*accept)(const char *),
                           u8 *data,
                           int &size)
{
  char buffer[2048];

  ArchiveList_struct *rarList = NULL;
  if(urarlib_list((void *)file, (ArchiveList_struct *)&rarList)) {
    ArchiveList_struct *p = rarList;
    
    bool found = false;
    while(p) {
      if(accept(p->item.Name)) {
        strcpy(buffer, p->item.Name);
        found = true;
        break;
      }
      p = p->next;
    }
    if(found) {
      void *memory = NULL;
      unsigned long lsize = 0;
      size = p->item.UnpSize;
      int r = urarlib_get((void *)&memory, &lsize, buffer, (void *)file, "");
      if(!r) {
        systemMessage(MSG_ERROR_READING_IMAGE,
                      N_("Error reading image %s"), buffer);
        urarlib_freelist(rarList);
        return NULL;
      }
      u8 *image = (u8 *)memory;
      if(data != NULL) {
        memcpy(image, data, size);
      }
      urarlib_freelist(rarList);
      return image;
    }
    systemMessage(MSG_NO_IMAGE_ON_ZIP,
                  N_("No image found on RAR file %s"), file);
    urarlib_freelist(rarList);
    return NULL;
  }
  // nothing found
  return NULL;
}
#endif

u8 *utilLoad(const char *file, //ichfly todo
             u8 *data,
             int &size,bool extram)
{
  u8 *image = data;


  FILE *f = fopen(file, "rb");

  if(!f) {
    systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error opening image %s"), file);
    return NULL;
  }

  fseek(f,0,SEEK_END);
  int fileSize = ftell(f);
  fseek(f,0,SEEK_SET);

  generatefilemap(fileSize);

  if(data == NULL)
  {
	  romSize = 0x02400000 - ((u32)sbrk(0) + 0x5000 + 0x2000);
	  rom = (u8 *)(sbrk(0) + 0x2000/*8K for futur alloc*/);              //rom = (u8 *)0x02180000; //old
	  image = data = rom;
	  size = romSize;
  }
  size_t read = fileSize <= size ? fileSize : size;


  size_t r= 0x80000;

  //workaround read
  /*int seek = 0;
  while(r == 0x80000)
  {
	fseek(f,seek,SEEK_SET);
	if(read > 0x80000)r = fread(image, 1, 0x80000, f); //512 KByte chucks
	else r = fread(image, 1, read, f);
	read -= r;
	image += r;
	seek += r;
	fclose(f);
	f = fopen(file, "rb"); //close and open
  }*/

	r = fread(image, 1, read, f);

#ifndef uppern_read_emulation
  fclose(f);
#else
  ichflyfilestream = f;
#endif

  if(r != read) {
    systemMessage(MSG_ERROR_READING_IMAGE,
                  N_("Error reading image %s"), file);
	while(1);
  }  
#ifdef uppern_read_emulation
  ichflyfilestreamsize = fileSize;
#endif
  //size = fileSize;
  
  return image;
}
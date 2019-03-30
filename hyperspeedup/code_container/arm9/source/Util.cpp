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

#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>
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
#include "bios.h"

#include "mydebuger.h"

#include "file_browse.h"

#include <nds.h>

#include "main.h"
#include "fatmore.h"
#include "fatfile.h"
#include "ds_dma.h"
#include <unistd.h>    // for sbrk()
#include <fat.h>


extern "C" {
//#include <png.h> //ichfly todo ganze datei!!!!!!!! realy todo
}


#include "ichflysettings.h"
#include "System.h"
#include "NLS.h"
#include "Util.h"
#include "Flash.h"
#include "GBA.h"
#include "RTC.h"

#ifndef _MSC_VER
#define _stricmp strcasecmp
#endif // ! _MSC_VER

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
int utilGetSize(int size)
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

void patchit(int romSize2)
{
	FILE *patchf = fopen(patchPath, "rb");
	u8 header[0x50];
	fread(header,1,0x50,patchf);
	if(memcmp(header,"flygbapatch",0xC))
	{
		systemMessage(MSG_ERROR_OPENING_IMAGE, N_("Error in patchfile"));
		while(1);
	}
	
	u32 * ptr = (u32*)&header[0x10];
	int patchnum = (*ptr);

	for(int i = 0;i < patchnum;i++)
	{
		int type;
		fread((void*)&type,1,0x4,patchf);
		switch (type)
		{
		case 0:
			{
			int offsetgba;
			int offsetthisfile;
			fread((void*)&offsetgba,1,0x4,patchf);
			fread((void*)&offsetthisfile,1,0x4,patchf);
			if(offsetgba + chucksize < romSize2)
			{
				int coo = ftell(patchf);
				fseek(patchf,offsetthisfile,SEEK_SET);
				fread(rom + offsetgba,1,chucksize,patchf);
				fseek(patchf,coo,SEEK_SET);
			}
			else
			{
				getandpatchmap(offsetgba,offsetthisfile);
			}
			}
			break;
		case 1:
			{
			fread((void*)&cheatsNumber,1,0x4,patchf);
			int offset;
			fread((void*)&offset,1,0x4,patchf);
			int coo5 = ftell(patchf);
			fseek(patchf,offset,SEEK_SET);
			fread((void*)cheatsList,1,cheatsNumber*28,patchf);
			fseek(patchf,coo5,SEEK_SET);
			__irqSet(IRQ_FIFO_NOT_EMPTY,arm7dmareqandcheat,irqTable);
			}
			break;
		case 2:
			{
			u32 gbaoffset;
			fread((void*)&gbaoffset,1,0x4,patchf);
			u32 payloadsize;
			fread((void*)&payloadsize,1,0x4,patchf);
			int offset;
			fread((void*)&offset,1,0x4,patchf);
			int coo = ftell(patchf);
			fseek(patchf,offset,SEEK_SET);
			fread((void*)gbaoffset,1,payloadsize,patchf);
			fseek(patchf,coo,SEEK_SET);
			break;
			}
		case 3:
			{
			u8 type;
			fread((void*)&type,1,0x1,patchf);
			u32 offset;
			fread((void*)&offset,1,0x4,patchf);
			int address;
			fread((void*)&address,1,0x4,patchf);
			u32 Condition;
			fread((void*)&Condition,1,0x4,patchf);
			if(offset & BIT(31))offset = (offset & ~BIT(31)) + (u32)rom;
			u32 topatchoffset = address - offset - 8;
			switch (type)
			{
				case 0:
					topatchoffset =+ 4;
					*(u16*)offset = (u16)0xF000 | (u16)((topatchoffset >> 12) & 0x7FF);
					*(u16*)(offset + 2) = (u16)0xF800 | (u16)((topatchoffset >> 1) & 0x7FF);
					break;
				case 1:
					topatchoffset =+ 4;
					*(u16*)offset = (u16)0xF000 + (u16)((topatchoffset >> 12) & 0x7FF);
					*(u16*)(offset + 2) = (u16)0xE800 + (u16)((topatchoffset >> 1) & 0x7FF);
					break;
				case 2:
					*(u32*)offset = (Condition << 0x1B) | 0x0A000000 | ((topatchoffset >> 2) & ~0xFF000000);
					break;
				case 3:
					*(u32*)offset = (Condition << 0x1B) | 0x0B000000 | ((topatchoffset >> 2) & ~0xFF000000);
					break;
			}
			}
			break;
			case 4:
			{
			u8 type;
			fread((void*)&type,1,0x1,patchf);
			u32 offset;
			fread((void*)&offset,1,0x4,patchf);
			int function=0;
			int address=0;
			fread((void*)&function,1,0x4,patchf);
			switch (function)
			{
				case 0:
					address = (u32)CPUReadMemorypu;
					break;
				case 1:
					address = (u32)CPUReadHalfWordpu;
					break;
				case 2:
					address = (u32)CPUReadBytepu;
					break;
				case 3:
					address = (u32)CPUWriteMemorypuextern;
					break;
				case 4:
					address = (u32)CPUWriteHalfWordpuextern;
					break;
				case 5:
					address = (u32)CPUWriteBytepuextern;
					break;
				case 6:
					address = (u32)CPUReadMemory;
					break;
				case 7:
					address = (u32)CPUReadHalfWord;
					break;
				case 8:
					address = (u32)CPUReadByte;
					break;
				case 9:
					address = (u32)CPUWriteMemoryextern;
					break;
				case 10:
					address = (u32)CPUWriteHalfWordextern;
					break;
				case 11:
					address = (u32)CPUWriteByteextern;
					break;
				case 12:
					address = (u32)CPUReadHalfWordSignedoutline;
					break;
				case 13:
					address = (u32)CPUReadByteSigned;
					break;
				case 14:
					address = (u32)CPUReadHalfWordrealpuSignedoutline;
					break;
				case 15:
					address = (u32)CPUReadByteSignedpu;
					break;
				case 100:
					address = (u32)ichfly_readu32extern;
					break;
				case 101:
					address = (u32)ichfly_readu16extern;
					break;
				case 102:
					address = (u32)ichfly_readu8extern;
					break;
			}
			u32 Condition;
			fread((void*)&Condition,1,0x4,patchf);
			if(offset & BIT(31))offset = (offset & ~BIT(31)) + (u32)rom;
			u32 topatchoffset = address - offset - 8;
			switch (type)
			{
				case 0:
					topatchoffset =+ 4;
					*(u16*)address = (u16)0xF000 | (u16)((topatchoffset >> 12) & 0x7FF);
					*(u16*)(address + 2) = (u16)0xF800 | (u16)((topatchoffset >> 1) & 0x7FF);
					break;
				case 1:
					topatchoffset =+ 4;
					*(u16*)address = (u16)0xF000 + (u16)((topatchoffset >> 12) & 0x7FF);
					*(u16*)(address + 2) = (u16)0xE800 + (u16)((topatchoffset >> 1) & 0x7FF);
					break;
				case 2:
					*(u32*)address = (Condition << 0x1B) | 0x0A000000 | ((topatchoffset >> 2) & ~0xFF000000);
					break;
				case 3:
					*(u32*)address = (Condition << 0x1B) | 0x0B000000 | ((topatchoffset >> 2) & ~0xFF000000);
					break;
			}
			}
			break;
			case 5:
				extern u8 VCountgbatods[0x100]; //(LY)      (0..227) + check overflow
				extern u8 VCountdstogba[263]; //(LY)      (0..262)
				extern u8 VCountdoit[263]; //jump in or out
				u32 offsetthisfile;
				fread((void*)&offsetthisfile,1,0x4,patchf);

				int coo = ftell(patchf);
				fseek(patchf,offsetthisfile,SEEK_SET);
				fread(VCountgbatods,1,0x100,patchf);
				fread(VCountdstogba,1,263,patchf);
				fread(VCountdoit,1,263,patchf);
				fseek(patchf,coo,SEEK_SET);
			break;
		}
	}
}

u8 *utilLoad(const char *file,
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
	fseek(f,0,SEEK_SET); //set at zero pos of GBA ROM block (offset = 0)
	
	generatefilemap(fileSize);
	
	#ifdef wifidebuger
	  romSize = 0x02380000 - ((u32)sbrk(0) + 0x5000 + 0x2000);
	#else
	  romSize = 	(int)0x02400000 - ((u32)sbrk(0) + 0x5000 + 0x2000);
	#endif

	image = rom = 		(u8 *)((u8 *)sbrk(0) + (int)0x2000);	
	size_t read = fileSize <= romSize ? fileSize : romSize;
	size_t r= 0x80000;
	if(cpuIsMultiBoot == true){	//MultiBoot cart?
		//read binary into workRAM and point the rom to WorkRAM so it executes there
		rom=workRAM;
	}
	else{	//Single Cart, Normal Boot
		r = fread(image, 1, read, f);
	}
	
	//set up header
    memcpy((u8*)&gbaheader, (u8*)image, sizeof(gbaHeader_t));
	
	#ifndef uppern_read_emulation
		fclose(f);
	#else
		ichflyfilestream = f; //pass the filestreampointer and make it global
		ichflyfilestreamsize = fileSize;
	#endif

	if(r != read) {
		systemMessage(MSG_ERROR_READING_IMAGE,
		N_("Error reading image %s"), file);
		while(1);
	}
	
	if(patchPath[0] != 0)patchit(romSize);
	
  return image;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//ARM core does not need them

// swaps a 16-bit value
/*
u16 swap16(u16 v)
{
  return (v<<8)|(v>>8);
}

// swaps a 32-bit value
u32 swap32(u32 v)
{
  return (v<<24)|((v<<8)&0xff0000)|((v>>8)&0xff00)|(v>>24);
}
*/

/*
#define CPUReadByteQuick(addr) \
  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]
*/

__attribute__((section(".itcm")))
u8 CPUReadByteQuick(u32 addr)	{
	return map[(addr)>>24].address[(addr) & map[(addr)>>24].mask];
}

/*
#define CPUReadHalfWordQuick(addr) \
  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))
*/

__attribute__((section(".itcm")))
u16 CPUReadHalfWordQuick(u32 addr){
	return READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]));
}
/*
#define CPUReadMemoryQuick(addr) \
  READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))
*/

__attribute__((section(".itcm")))
u32 CPUReadMemoryQuick(u32 addr){
	return READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]));
}


//little-endian GBA == little-endian NDS
__attribute__((section(".itcm")))
u16 READ16LE(u16 * x){
  return *((u16 *)x);
}

__attribute__((section(".itcm")))
u32 READ32LE(u32 * x){
  return *((u32 *)x);
}

__attribute__((section(".itcm")))
void WRITE16LE(u16 * x,u16 v){
  *((u16 *)x) = (v);
}

__attribute__((section(".itcm")))
void WRITE32LE(u32 * x, u32 v){
  *((u32 *)x) = (v);
}

__attribute__((section(".itcm")))
void UPDATE_REG(u16 address, u16 value){
	WRITE16LE(((u16*)&ioMem[address]),value);
}


char* strtoupper(char* s) {
  assert(s != NULL);

  char* p = s;
  while (*p != '\0') {
    *p = toupper(*p);
    p++;
  }

  return s;
}

char* strtolower(char* s) {
  assert(s != NULL);

  char* p = s;
  while (*p != '\0') {
    *p = tolower(*p);
    p++;
  }

  return s;
}


bool useMPUFast = false;

bool pendingSaveFix = false;	//false if already saved new forked save / or game doesn't meet savefix conditions // true if pending a save that was fixed in gba core, but still has not been written/updated to file.
int  SaveSizeBeforeFix = 0;	//only valid if pendingSaveFix == true
int  SaveSizeAfterFix = 0;	//only valid if pendingSaveFix == true

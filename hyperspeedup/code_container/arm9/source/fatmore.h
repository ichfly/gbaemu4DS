#include <nds.h>
#include <fat.h>

#include "ichflysettings.h"
 
#ifndef GBAEMU4DSSTRM
#define GBAEMU4DSSTRM

//Prototypes for ichfly's extended FAT stuff
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fatfile.h" //required for linking FILE_STRUCT and other FAT related initializers

//extra settings for ownfilebuffer
#define sectorscale 1 //1,2,4,8
#define buffslots 255
#define sectorsize 0x200*sectorscale
//#define sectorbinsz clzero(sectorsize)

//extra settings for ownfilebuffer
//#define chucksizeinsec 1 //1,2,4,8
//#define buffslots 255
//#define chucksize 0x200*chucksizeinsec


#define u32size (sizeof(u32))
#define u16size (sizeof(u16))
#define u8size (sizeof(u8))

#define sectorsize_u32units ((sectorsize/u32size)-1) //start from zero
#define sectorsize_u16units ((sectorsize/u16size)-1)
#define sectorsize_u8units ((sectorsize/u8size)-1)

#define sectorsize_int32units (sectorsize/u32size) 	//start from real number 1
#define sectorsize_int16units (sectorsize/u16size) 	//start from real number 1
#define sectorsize_int8units (sectorsize/u8size) 	//start from real number 1

#define strm_buf_size (int)(1024*256)

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef uppern_read_emulation
	extern FILE* ichflyfilestream;
	extern volatile int ichflyfilestreamsize;
#endif

extern __attribute__((section(".itcm"))) 	u8 ichfly_readu8extern(int pos);
extern __attribute__((section(".itcm"))) 	u16 ichfly_readu16extern(int pos);
extern __attribute__((section(".itcm"))) 	u32 ichfly_readu32extern(int pos);

extern void closegbarom();
extern void generatefilemap(int size);
extern void getandpatchmap(u32 offsetgba,u32 offsetthisfile);

extern volatile  u32 *sectortabel;
extern void * lastopen;
extern void * lastopenlocked;

extern PARTITION* partitionlocked;
extern FN_MEDIUM_READSECTORS	readSectorslocked;
extern u32 current_pointer;
extern u32 allocedfild[buffslots];
extern u8* greatownfilebuffer;

//stream fs (slow... nope)

/*
extern void free_map();
extern void generate_filemap(int size);
extern u32 stream_readu32(u32 pos);
extern u16 stream_readu16(u32 pos);
extern u8 stream_readu8(u32 pos);


//libfat / posix fat gba rom compatible
//extern FILE * gbaromfile; using FILE * ichflyfilestream instead

extern int startoffset32;	//start offset for disk_buf32 ,from gbabuffer fetch
extern int startoffset16;	//start offset for disk_buf32 ,from gbabuffer fetch
extern int startoffset8;	//start offset for disk_buf32 ,from gbabuffer fetch

u32 isgbaopen(FILE * gbahandler);
u32 opengbarom(const char * filename,const char * access_type);
u32 closegbarom();
u32 ichfly_readu32(int offset);
u16 ichfly_readu16(int offset);
u8 ichfly_readu8(int offset);

u16 writeu16gbarom(int offset,u16 * buf_in,int size_elem);
u32 writeu32gbarom(int offset,u32 * buf,int size);
u32 getfilesizegbarom();
u32 copygbablock(int offset,u8 mode); //mode 0:32b \ 1:16b \ 2:8b

//[nds] disk sector ram buffer
extern volatile u32 disk_buf32[(strm_buf_size)]; 	//32 reads //0x80
extern volatile u16 disk_buf16[(strm_buf_size)];	//16 reads
extern volatile u8 disk_buf8[(strm_buf_size)];		//16 reads

//filesystem nds cluster size
extern u32 ndsclustersize;

//filemapsize 
extern u32 fmapsize;
*/


static inline u8 ichfly_readu8(int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	int sectoroffset = pos % sectorsize;
	int mappoffset = pos / sectorsize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2 + 1]);
	
	if(asd != (u8*)0x0)return asd[sectoroffset]; //found exit here

	sectortabel[allocedfild[current_pointer]] = 0x0; //reset

	allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
	asd = greatownfilebuffer + current_pointer * sectorsize;
	sectortabel[mappoffset*2 + 1] = (u32)asd;

	readSectorslocked(sectortabel[mappoffset*2], sectorscale, asd);
	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return *(u8*)(&asd[sectoroffset]);
}


static inline u16 ichfly_readu16(int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	int sectoroffset = pos % sectorsize;
	int mappoffset = pos / sectorsize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2 + 1]);
	
	if(asd != (u8*)0x0)return *(u16*)(&asd[sectoroffset]); //found exit here

	sectortabel[allocedfild[current_pointer]] = 0x0; //clear old slot

	allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
	asd = greatownfilebuffer + current_pointer * sectorsize;
	sectortabel[mappoffset*2 + 1] = (u32)asd;
	
	readSectorslocked(sectortabel[mappoffset*2], sectorscale, asd);

	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return *(u16*)(&asd[sectoroffset]);
}


static inline u32 ichfly_readu32(int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	int sectoroffset = pos % sectorsize;
	int mappoffset = pos / sectorsize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2 + 1]);
	
	if(asd != (u8*)0x0)return *(u32*)(&asd[sectoroffset]); //found exit here

	sectortabel[allocedfild[current_pointer]] = 0x0;

	allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
	asd = greatownfilebuffer + current_pointer * sectorsize;
	sectortabel[mappoffset*2 + 1] = (u32)asd;

	readSectorslocked(sectortabel[mappoffset*2], sectorscale, asd);
	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return *(u32*)(&asd[sectoroffset]);
}
static inline void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal) //need lockup only alined is not working 
{

	// Calculate the sector and byte of the current position,
	// and store them
	int sectoroffset = 0;
	int mappoffset = 0;

	int currsize = 0;

	if(readal == 4) //32 Bit
	{
		while(c > 0)
		{
			sectoroffset = (pos % sectorsize) /4;
			mappoffset = pos / sectorsize;
			currsize = (sectorsize / 4) - sectoroffset;
			if(currsize == 0)currsize = sectorsize / 4;
			if(currsize > c) currsize = c;
			

			u32* asd = (u32*)(sectortabel[mappoffset*2 + 1]);
			
			if(asd != (u32*)0x0)//found exit here
			{
				int i = 0; //copy
				while(currsize > i)
				{
					*(u32*)(&ptr[i*4]) = asd[sectoroffset + i];
					i++;
				}
				c -= currsize;
				pos += (currsize * 4);
				ptr += (currsize * 4);
				continue;
			}

			sectortabel[allocedfild[current_pointer]] = 0x0;

			allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
			asd = (u32*)(greatownfilebuffer + current_pointer * sectorsize);
			sectortabel[mappoffset*2 + 1] = (u32)asd;

			readSectorslocked(sectortabel[mappoffset*2], sectorscale, asd);
			current_pointer++;
			if(current_pointer == buffslots)current_pointer = 0;

			int i = 0; //copy
			while(currsize > i)
			{
				*(u32*)(&ptr[i*4]) = asd[sectoroffset + i];
				i++;
			}
			c -= currsize;
			pos += (currsize * 4);
			ptr += (currsize * 4);
		}
	}
	else //16 Bit
	{
		while(c > 0)
		{
			sectoroffset = (pos % sectorsize) / 2;
			mappoffset = pos / sectorsize;
			currsize = (sectorsize / 2) - sectoroffset;
			if(currsize == 0)currsize = sectorsize / 2;
			if(currsize > c) currsize = c;

			u16* asd = (u16*)(sectortabel[mappoffset*2 + 1]);
			//iprintf("%X %X %X %X %X %X\n\r",sectoroffset,mappoffset,currsize,pos,c,sectorsize);
			if(asd != (u16*)0x0)//found exit here
			{
				int i = 0; //copy
				while(currsize > i)
				{
					*(u16*)(&ptr[i*2]) = asd[sectoroffset + i];
					i++;
				}
				c -= currsize;
				ptr += (currsize * 2);
				pos += (currsize * 2);
				continue;
			}

			sectortabel[allocedfild[current_pointer]] = 0x0;

			allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
			asd = (u16*)(greatownfilebuffer + current_pointer * sectorsize);
			sectortabel[mappoffset*2 + 1] = (u32)asd;

			readSectorslocked(sectortabel[mappoffset*2], sectorscale, asd);
			current_pointer++;
			if(current_pointer == buffslots)current_pointer = 0;

			int i = 0; //copy
			while(currsize > i)
			{
				*(u16*)(&ptr[i*2]) = asd[sectoroffset + i];
				i++;
			}
			c -= currsize;
			ptr += (currsize * 2);
			pos += (currsize * 2);
		}

	}
}

#ifdef __cplusplus
}
#endif

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

extern __attribute__((section(".itcm")))  	u8 ichfly_readu8(int pos);
extern __attribute__((section(".itcm")))  	u16 ichfly_readu16(int pos);
extern __attribute__((section(".itcm")))  	u32 ichfly_readu32(int pos);
extern __attribute__((section(".itcm")))	void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal);

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

#ifdef __cplusplus
}
#endif

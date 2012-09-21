
#include "fatfile.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include "cache.h"
#include "file_allocation_table.h"
#include "bit_ops.h"
#include "filetime.h"
#include "lock.h"

#include "ichflysettings.h"

#include "System.h"

extern u32 *sectortabel;

extern void * lastopen;

extern void * lastopenlocked;



extern PARTITION* partitionlocked;
extern FN_MEDIUM_READSECTORS	readSectorslocked;
extern u32 current_pointer;
extern u32 allocedfild[buffslots];
extern u8* greatownfilebuffer;
static inline u8 ichfly_readu8(int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	int sectoroffset = pos % chucksize;
	int mappoffset = pos / chucksize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2 + 1]);
	
	if(asd != (u8*)0xFFFFFFFF)return asd[sectoroffset]; //found exit here

	sectortabel[allocedfild[current_pointer]] = 0xFFFFFFFF; //reset

	allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
	asd = greatownfilebuffer + current_pointer * chucksize;
	sectortabel[mappoffset*2 + 1] = (u32)asd;

	readSectorslocked(sectortabel[mappoffset*2], chucksizeinsec, asd);
	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return asd[sectoroffset];
}
static inline u16 ichfly_readu16(int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	int sectoroffset = pos % chucksize;
	int mappoffset = pos / chucksize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2 + 1]);
	
	if(asd != (u8*)0xFFFFFFFF)return *(u16*)(&asd[sectoroffset]); //found exit here

	sectortabel[allocedfild[current_pointer]] = 0xFFFFFFFF; //clear old slot

	allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
	asd = greatownfilebuffer + current_pointer * chucksize;
	sectortabel[mappoffset*2 + 1] = (u32)asd;
	
	readSectorslocked(sectortabel[mappoffset*2], chucksizeinsec, asd);

	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return *(u16*)(&asd[sectoroffset]);
}
static inline u32 ichfly_readu32(int pos) //need lockup
{

	// Calculate the sector and byte of the current position,
	// and store them
	int sectoroffset = pos % chucksize;
	int mappoffset = pos / chucksize;
	
	u8* asd = (u8*)(sectortabel[mappoffset*2 + 1]);
	
	if(asd != (u8*)0xFFFFFFFF)return *(u32*)(&asd[sectoroffset]); //found exit here

	sectortabel[allocedfild[current_pointer]] = 0xFFFFFFFF;

	allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
	asd = greatownfilebuffer + current_pointer * chucksize;
	sectortabel[mappoffset*2 + 1] = (u32)asd;

	readSectorslocked(sectortabel[mappoffset*2], chucksizeinsec, asd);
	current_pointer++;
	if(current_pointer == buffslots)current_pointer = 0;
	
	return *(u32*)(&asd[sectoroffset]);
}
inline void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal) //need lockup only alined is not working 
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
			sectoroffset = (pos % chucksize) /4;
			mappoffset = pos / chucksize;
			currsize = (chucksize / 4) - sectoroffset;
			if(currsize == 0)currsize = chucksize / 4;
			if(currsize > c) currsize = c;
			

			u32* asd = (u32*)(sectortabel[mappoffset*2 + 1]);
			
			if(asd != (u32*)0xFFFFFFFF)//found exit here
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

			sectortabel[allocedfild[current_pointer]] = 0xFFFFFFFF;

			allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
			asd = (u32*)(greatownfilebuffer + current_pointer * chucksize);
			sectortabel[mappoffset*2 + 1] = (u32)asd;

			readSectorslocked(sectortabel[mappoffset*2], chucksizeinsec, asd);
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
			sectoroffset = (pos % chucksize) / 2;
			mappoffset = pos / chucksize;
			currsize = (chucksize / 2) - sectoroffset;
			if(currsize == 0)currsize = chucksize / 2;
			if(currsize > c) currsize = c;

			u16* asd = (u16*)(sectortabel[mappoffset*2 + 1]);
			//iprintf("%X %X %X %X %X %X\n\r",sectoroffset,mappoffset,currsize,pos,c,chucksize);
			if(asd != (u16*)0xFFFFFFFF)//found exit here
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

			sectortabel[allocedfild[current_pointer]] = 0xFFFFFFFF;

			allocedfild[current_pointer] = mappoffset*2 + 1; //set new slot
			asd = (u16*)(greatownfilebuffer + current_pointer * chucksize);
			sectortabel[mappoffset*2 + 1] = (u32)asd;

			readSectorslocked(sectortabel[mappoffset*2], chucksizeinsec, asd);
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

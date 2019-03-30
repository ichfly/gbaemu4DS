#include <nds.h>
#include <fat.h>

#include "fatmore.h"
#include "fatfile.h"
#include "file_allocation_table.h"
#include "disc.h"
#include "partition.h"

#include "ichflysettings.h"

//gbaemu4ds file stream code

#ifdef uppern_read_emulation
	FILE* ichflyfilestream;
	
	volatile int ichflyfilestreamsize=0;
#endif


volatile u32 *sectortabel;
void * lastopen;
void * lastopenlocked;

PARTITION* partitionlocked;
FN_MEDIUM_READSECTORS	readSectorslocked;
u32 current_pointer = 0;
u32 allocedfild[buffslots];
u8* greatownfilebuffer;


__attribute__((section(".itcm")))
__attribute__ ((aligned (4)))
u8 ichfly_readu8extern(int pos){
return ichfly_readu8(pos); //ichfly_readu8(pos);
}

__attribute__((section(".itcm")))
__attribute__ ((aligned (4)))
u16 ichfly_readu16extern(int pos){
return ichfly_readu16(pos); //ichfly_readu16(pos);
}

__attribute__((section(".itcm")))
__attribute__ ((aligned (4)))
u32 ichfly_readu32extern(int pos){
return ichfly_readu32(pos); //ichfly_readu32(pos);
}

//gbaemu4ds ichfly stream code

void closegbarom(){
	if(!ichflyfilestream){
		iprintf("FATAL: GBAFH isn't open");
	}

	fclose(ichflyfilestream);

	iprintf("GBARom closed! \n");
}

void generatefilemap(int size)
{
	FILE_STRUCT* file = (FILE_STRUCT*)(lastopen);
	lastopenlocked = lastopen; //copy
	PARTITION* partition;
	uint32_t cluster;
	int clusCount;
	partition = file->partition;
	partitionlocked = partition;

	readSectorslocked = file->partition->disc->readSectors;
	iprintf("generating file map (size %d Byte)",((size/sectorsize) + 1)*8);
	sectortabel =(u32*)malloc(((size/sectorsize) + 1)*8); //alloc for size every Sector has one u32
	greatownfilebuffer =(u8*)malloc(sectorsize * buffslots);

	clusCount = size/partition->bytesPerCluster;
	cluster = file->startCluster;


	//setblanc
	int i = 0;
	while(i < (partition->bytesPerCluster/sectorsize)*clusCount+1)
	{
		sectortabel[i*2 + 1] = 0x0;
		i++;
	}
	i = 0;
	while(i < buffslots)
	{
		allocedfild[i] = 0x0;
		i++;
	}


	int mappoffset = 0;
	i = 0;
	while(i < (partition->bytesPerCluster/sectorsize))
	{
		sectortabel[mappoffset*2] = _FAT_fat_clusterToSector(partition, cluster) + i;
		
		//debugging (fat fs sector numbers of image rom) 
		//iprintf("(%d)[%x]",(int)i,(unsigned int)_FAT_fat_clusterToSector(partition, cluster) + i);
		
		mappoffset++;
		i++;
	}
	while (clusCount > 0) {
		clusCount--;
		cluster = _FAT_fat_nextCluster (partition, cluster);

		i = 0;
		while(i < (partition->bytesPerCluster/sectorsize))
		{
			sectortabel[mappoffset*2] = _FAT_fat_clusterToSector(partition, cluster) + i;
			mappoffset++;
			i++;
		}
	}

}

void getandpatchmap(u32 offsetgba,u32 offsetthisfile)
{
	FILE_STRUCT* file = (FILE_STRUCT*)(lastopen);
	PARTITION* partition;
	uint32_t cluster;
	int clusCount;
	partition = file->partition;

	clusCount = offsetthisfile/partition->bytesPerCluster;
	cluster = file->startCluster;

	int offset1 = (offsetthisfile/sectorsize) % partition->bytesPerCluster;

	int mappoffset = offsetthisfile/sectorsize;
	while (clusCount > 0) {
		clusCount--;
		cluster = _FAT_fat_nextCluster (partition, cluster);
	}
	sectortabel[mappoffset*2] = _FAT_fat_clusterToSector(partition, cluster) + offset1;
}
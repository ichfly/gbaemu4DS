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

__attribute__((section(".itcm")))
__attribute__ ((aligned (4)))
inline u8 ichfly_readu8(int pos) //need lockup
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

__attribute__((section(".itcm")))
__attribute__ ((aligned (4)))
inline u16 ichfly_readu16(int pos) //need lockup
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

__attribute__((section(".itcm")))
__attribute__ ((aligned (4)))
inline u32 ichfly_readu32(int pos) //need lockup
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

__attribute__((section(".itcm")))
__attribute__ ((aligned (4)))
void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal) //need lockup only alined is not working 
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

//ichfly end

///////////////////////////////////////////////////////coto 1 (slow) begin
/*
inline u8 ichfly_readu8(int pos){
//very important to link
extern u32 * sectortabel;

//sector offset
//offsetting was right, sector storing cluster didn't start from zero!

//ori: unsigned int sectoroffset = pos % sectorsize;
unsigned int sectoroffset = (pos) % (sectorsize);

//sector pos
//ori: unsigned int mappoffset = pos / sectorsize;
unsigned int mappoffset = (pos) / (sectorsize);

//required so don't corrupt unallocated memory, and allocate 512 byte buffer for fetching data
u8 tempbuf[sectorsize];
u8* asd = (u8*)&tempbuf[0];

//get offset from sectortable
readSectorslocked(sectortabel[mappoffset], sectorscale, asd);

//debug
//iprintf("partition->bytesPerCluster:%x \n",ndsclustersize);
//iprintf("buffersectorload content: @pos:%x \n",pos);
//iprintf("sector(%x):[%x]",sectortabel[sectoroffset], *(u32*)(&asd[sectoroffset]));

return *(u8*)(&asd[sectoroffset]);
}

inline u16 ichfly_readu16(int pos){
//very important to link
extern u32 * sectortabel;

//sector offset
//offsetting was right, sector storing cluster didn't start from zero!

//ori: unsigned int sectoroffset = pos % sectorsize;
unsigned int sectoroffset = (pos) % (sectorsize);

//sector pos
//ori: unsigned int mappoffset = pos / sectorsize;
unsigned int mappoffset = (pos) / (sectorsize);

//required so don't corrupt unallocated memory, and allocate 512 byte buffer for fetching data
u8 tempbuf[sectorsize];
u8* asd = (u8*)&tempbuf[0];

//get offset from sectortable
readSectorslocked(sectortabel[mappoffset], sectorscale, asd);

//debug
//iprintf("partition->bytesPerCluster:%x \n",ndsclustersize);
//iprintf("buffersectorload content: @pos:%x \n",pos);
//iprintf("sector(%x):[%x]",sectortabel[sectoroffset], *(u32*)(&asd[sectoroffset]));

return *(u16*)(&asd[sectoroffset]);
}

inline u32 ichfly_readu32(int pos){
//very important to link
extern u32 * sectortabel;

//sector offset
//offsetting was right, sector storing cluster didn't start from zero!

//ori: unsigned int sectoroffset = pos % sectorsize;
unsigned int sectoroffset = (pos) % (sectorsize);

//sector pos
//ori: unsigned int mappoffset = pos / sectorsize;
unsigned int mappoffset = (pos) / (sectorsize);

//required so don't corrupt unallocated memory, and allocate 512 byte buffer for fetching data
u8 tempbuf[sectorsize];
u8* asd = (u8*)&tempbuf[0];

//get offset from sectortable
readSectorslocked(sectortabel[mappoffset], sectorscale, asd);

//debug
//iprintf("partition->bytesPerCluster:%x \n",ndsclustersize);
//iprintf("buffersectorload content: @pos:%x \n",pos);
//iprintf("sector(%x):[%x]",sectortabel[sectoroffset], *(u32*)(&asd[sectoroffset]));

return *(u32*)(&asd[sectoroffset]);
}

//dma copy stream
inline void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal) //need lockup only alined is not working 
{
	
}

//filesystem nds cluster size
u32 ndsclustersize=0;

//filemapsize 
u32 fmapsize=0;

void generatefilemap(int size){
//very important to link
extern u32 * sectortabel;
	
	FILE_STRUCT* file = (FILE_STRUCT*)(lastopen);
	lastopenlocked = lastopen; 	//copy
	PARTITION* partition;
	
	partition = file->partition;
	partitionlocked = partition;
	readSectorslocked = file->partition->disc->readSectors;
	
	iprintf("Generating Filemap: (sectortable size:[0x%x]) \n ",(size/sectorsize)*sizeof(u32)); //u32 because sector length is u32
	fmapsize=(size/sectorsize)*sizeof(u32);
	sectortabel = (u32*)malloc((size/sectorsize)*sizeof(u32)); //filemap = (romsize / sectorsize) * sizeof(u32)
	
	//Cluster file start offset is file->startCluster
	
	u32 cluster = 0;
	u32 clusCount=0;
	int i=0,mappoffset=0;
	ndsclustersize=partition->bytesPerCluster;
	
	//tmp
	//u8 tempbuf[sectorsize];
	//u8* fetch = (u8*)&tempbuf[0];
	
	//for each file's cluster..
	for(clusCount=0; clusCount < size/(partition->bytesPerCluster); clusCount++){
		//Next Cluster.. (clusters must be checked because they may not be linear)
		//starts from zero
		cluster = _FAT_fat_nextCluster(partition, (file->startCluster) + clusCount) - 1;
		
		//And retrieve 1 cluster : N sector into sectortabel
		for(i=0; i < (((int)partition->bytesPerCluster)/sectorsize) ;i++){
			sectortabel[mappoffset] = _FAT_fat_clusterToSector(partition, cluster) + i;
			
			//if (i==0){ //debug
			//	iprintf("first sector is: %x",sectortabel[i]); 
			//	iprintf("first cluster:%x\n",file->startCluster + clusCount);
			//	readSectorslocked(sectortabel[mappoffset / sectorsize], sectorscale, fetch);
			//	iprintf("and data[%x]",*(u32*)(&fetch[i % sectorsize]));
			//	while(1);
			//}
			mappoffset++;
		}
	}
	//debug
	//iprintf("File props: \n");
	//iprintf("ROM Clusters:[0x%x] \n",size/(partition->bytesPerCluster));
	//iprintf("ROM Filemap Cluster Size:[0x%x] \n",(((int)partition->bytesPerCluster)/sectorsize));
	//iprintf("partition->bytesPerCluster:%x \n",partition->bytesPerCluster);
	//iprintf("OK! \n");
	
}

///////////////////////////////////////////////////////coto 1 (slow) end
*/



//////////////////////////////////////////////////////coto 2 begin (slow, nope)

/*
//[nds] disk sector ram buffer
volatile u32 disk_buf32[(strm_buf_size)]; 	//32 reads //0x80
volatile u16 disk_buf16[(strm_buf_size)];	//16 reads
volatile u8 disk_buf8[(strm_buf_size)];		//16 reads


//FILE * gbaromfile; //using ichflyfilestream instead

int startoffset32=0;	//start offset for disk_buf32 ,from gbabuffer fetch
int startoffset16=0;	//start offset for disk_buf32 ,from gbabuffer fetch
int startoffset8=0;	//start offset for disk_buf32 ,from gbabuffer fetch

u32 isgbaopen(FILE * gbahandler){

	if (!gbahandler)
		return 1;
	else
		return 0;
}

//mode	Description
//"r"	Open a file for reading. The file must exist.
//"w"	Create an empty file for writing. If a file with the same name already exists its content is erased and the file is considered as a new empty file.
//"a"	Append to a file. Writing operations append data at the end of the file. The file is created if it does not exist.
//"r+"	Open a file for update both reading and writing. The file must exist.
//"w+"	Create an empty file for both reading and writing.
//"a+"	Open a file for reading and appending.
//

u32 opengbarom(const char * filename,const char * access_type){

	FILE *fh = fopen(filename, access_type); //r
	if(!fh)
		return 1;

	ichflyfilestream=fh;
return 0;
}

u32 closegbarom(){
	if(!ichflyfilestream){
		iprintf("FATAL: GBAFH isn't open");
		return 1;
	}

	fclose(ichflyfilestream);

	iprintf("GBARom closed!");
return 0;
}

//offset to copy, mode=0 / 32bit | mode=1 / 16 bit buffer copies
u32 copygbablock(int offset,u8 mode){

	if(!ichflyfilestream){
		iprintf("FATAL: GBAFH isn't open");
		return 0;
	}
	
	int sizeread=0;
	
	switch(mode){
		//32bit copies
		case 0:{
			//1) from start of file where (offset)
			//int fseek(FILE *stream, long int offset, int whence);
			fseek(ichflyfilestream,(long int)offset, SEEK_SET); 					
		
			//2) perform read (512bytes read (128 reads))
			sizeread=fread((void*)disk_buf32, (int)u32size, (int)sectorsize_int32units ,ichflyfilestream);
		
			if (sizeread!=(int)sectorsize_int32units){
				iprintf("FATAL: GBAREAD isn't (%d) bytes",(int)sectorsize_int32units);
				return 0;
			}
			//3) and set pointer to what it was
			fseek(ichflyfilestream,0, SEEK_SET);									
		}
		break;
		
		//16bit copies
		case 1:{
			//1) from start of file where (offset)
			//int fseek(FILE *stream, long int offset, int whence);
			fseek(ichflyfilestream,(long int)offset, SEEK_SET); 					
		
			//2) perform read (512bytes read (256 reads))
			sizeread=fread((void*)disk_buf16, (int)u16size, (int)sectorsize_int16units ,ichflyfilestream);
		
			if (sizeread!=(int)sectorsize_int16units){
				iprintf("FATAL: GBAREAD isn't (%d) bytes, but: %x ",(int)sectorsize_int16units,(int)sizeread);
				return 0;
			}
			//3) and set pointer to what it was
			fseek(ichflyfilestream,0, SEEK_SET);									
		}
		break;
		
		//8bit copies
		case 2:{
			//1) from start of file where (offset)
			//int fseek(FILE *stream, long int offset, int whence);
			fseek(ichflyfilestream,(long int)offset, SEEK_SET); 					
		
			//2) perform read (512bytes read (256 reads))
			sizeread=fread((void*)disk_buf8, (int)u8size, (int)sectorsize_int8units ,ichflyfilestream);
		
			if (sizeread!=(int)sectorsize_int8units){
				iprintf("FATAL: GBAREAD isn't (%d) bytes, but: %x ",(int)sectorsize_int8units,(int)sizeread);
				return 0;
			}
			//3) and set pointer to what it was
			fseek(ichflyfilestream,0, SEEK_SET);
		}
		break;
		
		default:
			sizeread=0;
		break;
	}
	
return (u32)sizeread;
}

u8 ichfly_readu8(int offset){
	if 	( 	(offset < (int)(startoffset8+sectorsize)) //starts from zero, so startoffset+sectorsize is next element actually((n-1)+1).
			&&
			(offset	>=	startoffset8)
		){
		return disk_buf8[ ((offset - startoffset8) / u8size) ]; //OK
	}
	else{
		int outblk=copygbablock(offset,2);
		if(((int)sectorsize_int8units)!=outblk){
			iprintf("\n readu8gbarom(); error copying romdata into disk_buf8");
			while(1);
			return 0;
		}
	
		startoffset8=offset;
		return disk_buf8[0]; //OK (must be zero since element 0 of the newly filled gbarom buffer has offset contents)
	}
return 0;
}

u16 ichfly_readu16(int offset){

	if 	( 	(offset < (int)(startoffset16+sectorsize)) //starts from zero, so startoffset+sectorsize is next element actually((n-1)+1).
			&&
			(offset	>=	startoffset16)
		){
		
		return disk_buf16[ ((offset - startoffset16) / u16size) ]; //OK
	}
	else{
		int outblk=copygbablock(offset,1);
		if(((int)sectorsize_int16units)!=outblk){
			iprintf("\n readu16gbarom(); error copying romdata into disk_buf16");
			while(1);
			return 0;
		}
	
		startoffset16=offset;
		return disk_buf16[0]; //OK (must be zero since element 0 of the newly filled gbarom buffer has offset_start)
	}
return 0;
}


u32 ichfly_readu32(int offset){

	if 	( 	(offset < (int)(startoffset32+sectorsize)) //starts from zero, so startoffset+sectorsize is next element actually((n-1)+1).
			&&
			(offset	>=	startoffset32)
		){
		return disk_buf32[ ((offset - startoffset32) / u32size) ]; //OK		
	}
	else{
		int outblk=copygbablock(offset,0);
		if(((int)sectorsize_int32units)!=outblk){
			iprintf("\n readu32gbarom(); error copying romdata into disk_buf32");
			while(1);
			return 0;
		}
	
		startoffset32=offset;
		return disk_buf32[0]; //OK (must be zero since element 0 of the newly filled gbarom buffer has offset_start)
	}
return 0;
}


u16 writeu16gbarom(int offset,u16 * buf_in,int size_elem){

	if(!ichflyfilestream){
		iprintf("FATAL: GBAFH isn't open");
		return 1;
	}
	iprintf("\n trying to write: %x",(unsigned int)buf_in[0x0]);

	//int fseek(FILE *stream, long int offset, int whence);
	fseek(ichflyfilestream,(long int)offset, SEEK_SET); 					//1) from start of file where (offset)

	//size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
	int sizewritten=fwrite((u16*)buf_in, 1, size_elem, ichflyfilestream); //2) perform read (512bytes read (128 reads))
	if (sizewritten!=size_elem){
		iprintf("FATAL: GBAWRITE isn't (%d) bytes, instead: (%x) bytes",(int)size_elem,(int)sizewritten);
	}
	else{
		iprintf("write ok!");
	}
	
	fseek(ichflyfilestream,0, SEEK_SET);									//3) and set pointer to what it was

return 0;
}


u32 writeu32gbarom(int offset,u32 * buf_in,int size_elem){

	if(!ichflyfilestream){
		iprintf("FATAL: GBAFH isn't open");
		return 1;
	}
	iprintf("\n trying to write: %x",(unsigned int)buf_in[0x0]);

	//int fseek(FILE *stream, long int offset, int whence);
	fseek(ichflyfilestream,(long int)offset, SEEK_SET); 					//1) from start of file where (offset)

	//size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
	int sizewritten=fwrite((u32*)buf_in, 1, size_elem, ichflyfilestream); //2) perform read (512bytes read (128 reads))
	if (sizewritten!=size_elem){
		iprintf("FATAL: GBAWRITE isn't (%d) bytes, instead: (%x) bytes",(int)size_elem,(int)sizewritten);
	}
	else{
		iprintf("write ok!");
	}
	
	fseek(ichflyfilestream,0, SEEK_SET);									//3) and set pointer to what it was

return 0;
}

u32 getfilesizegbarom(){
	if(!ichflyfilestream){
		iprintf("FATAL: GBAFH isn't open");
		return 0;
	}
	fseek(ichflyfilestream,0,SEEK_END);
	int filesize = ftell(ichflyfilestream);
	fseek(ichflyfilestream,0,SEEK_SET);

return filesize;
}
*/

///////////////////////////////////////////////////////coto 2 end

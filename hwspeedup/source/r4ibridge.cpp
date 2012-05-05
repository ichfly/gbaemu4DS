//#define CPUReadByteQuick(addr) \
//  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]

//a

#include "System.h"
#include "Port.h"
#include "RTC.h"
#include "r4i.h"
#include "globals.h"
#include "nds.h"
int buff1 = 0;
int buff2 = 0;
int currentfull = 0;

FILE *f;

u16 buffmap[MEM80bufferslots];

//ichfly from GBAinline.h

//ichfly my function must be adapted to r4i to get real speed

void r4iopen(const char* path)
{
	f = fopen(path,"r");
	printf("%x %x %x\r\n",f,path,*(u32*)path);
}
int dmagetloaded()
{
	return 0;
}
void dmastartloadin(unsigned int addr)
{
	fseek(f,(addr & (~cucksize & 0x01FFFFFF)),SEEK_SET);


	if(currentfull != MEM80bufferslots)
	{
		fread((void*)(0x02200000 + MEM80bufferslotssize * currentfull), 1, MEM80bufferslotssize, f);
		
		printf("%x %x %x\r\n",addr,(addr & (~cucksize & 0x01FFFFFF)),0x02200000 + MEM80bufferslotssize * currentfull);

		map[((addr)>>mapseekoffs) & mapander].address = (u8*)(0x02200000 + MEM80bufferslotssize * currentfull);
		buffmap[currentfull] = ((addr)>>mapseekoffs) & mapander;
		currentfull++;
	}
	else
	{
		int random = rand() % MEM80bufferslots;
		map[buffmap[random]].status = 2;
		fread((void*)0x02200000 + MEM80bufferslotssize * random, 1, MEM80bufferslotssize, f);
		map[((addr)>>mapseekoffs) & mapander].address = (u8*)0x02200000 + MEM80bufferslotssize * random;
		buffmap[random] = ((addr)>>mapseekoffs) & mapander;
	}
	map[((addr)>>mapseekoffs) & mapander].status = 0;
	map[((addr)>>mapseekoffs) & mapander].loaded = MEM80bufferslotssize;
}

#define dbg_printf(...) iprintf(__VA_ARGS__)

u8 CPUReadByteQuick(u32 addr) //ichfly here rom reader 
{
	//printf("0 %x\r\n",addr);
	if(map[((addr)>>mapseekoffs) & mapander].status == 0) // 0 = full loaded
	{
		return map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask];
	}
	else
	{
		if(map[((addr)>>mapseekoffs) & mapander].status == 1) //1 = load in progress
		{
			while(map[((addr)>>mapseekoffs) & mapander].loaded + dmagetloaded() < (addr & cucksize) + 1); //sorry you need to wait :(
			return map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask];
		}
		if(map[((addr)>>mapseekoffs) & mapander].status == 2) //2 = not loaded
		{
			dmastartloadin(addr);
			while(map[((addr)>>mapseekoffs) & mapander].loaded + dmagetloaded() < (addr & cucksize) + 1); //sorry you need to wait :(
			return map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask];
		}
	}
}

//#define CPUReadHalfWordQuick(addr) \
//  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

u16 CPUReadHalfWordQuick(u32 addr) //ichfly here rom reader
{
	//printf("1 %x\r\n",addr);
	if(map[((addr)>>mapseekoffs) & mapander].status == 0) // 0 = full loaded
	{
		return READ16LE((u16*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
	}
	else
	{
		if(map[((addr)>>mapseekoffs) & mapander].status == 1) //1 = load in progress
		{
			while(map[((addr)>>mapseekoffs) & mapander].loaded + dmagetloaded() < (addr & cucksize) + 2); //sorry you need to wait :(
			return READ16LE((u16*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
		}
		if(map[((addr)>>mapseekoffs) & mapander].status == 2) //2 = not loaded
		{
			dmastartloadin(addr);
			while(map[((addr)>>mapseekoffs) & mapander].loaded + dmagetloaded() < (addr & cucksize) + 2); //sorry you need to wait :(
			return READ16LE((u16*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
		}
	}
}

//#define CPUReadMemoryQuick(addr) \
//  READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))
u32 CPUReadMemoryQuick(u32 addr) //ichfly here rom reader
{
	
	/*if(addr < 0x02000000)
	{
		printf("2 %x\r\n",addr);
		while(1);
	}*/
	if(map[((addr)>>mapseekoffs) & mapander].status == 0) // 0 = full loaded
	{
		//printf("0 %x %x %x\r\n",addr,((addr)>>mapseekoffs) & mapander,READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]));
		//printf("2 %x %x %x\r\n",addr,READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]),map[((addr)>>mapseekoffs) & mapander].address);
		return READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
	}
	else
	{
		if(map[((addr)>>mapseekoffs) & mapander].status == 1) //1 = load in progress
		{
			while(map[((addr)>>mapseekoffs) & mapander].loaded + dmagetloaded() < (addr & cucksize) + 4); //sorry you need to wait :(
			//printf("1 %x %x %x\r\n",addr,map[((addr)>>mapseekoffs) & mapander].address,READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]));
			//printf("2 %x %x %x\r\n",addr,READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]),&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask],);
			return READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
		}
		if(map[((addr)>>mapseekoffs) & mapander].status == 2) //2 = not loaded
		{

			dmastartloadin(addr);

			while(map[((addr)>>mapseekoffs) & mapander].loaded + dmagetloaded() < (addr & cucksize) + 4); //sorry you need to wait :(
			//printf("2 %x %x %x\r\n",addr,READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]),&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);
			//while(1);

			//printf("2 %x %x\r\n",addr,READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]));
			return READ32LE((u32*)&map[((addr)>>mapseekoffs) & mapander].address[(addr) & map[((addr)>>mapseekoffs) & mapander].mask]);

		}
	}
}
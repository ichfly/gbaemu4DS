#include <nds.h>
#include <stdio.h>
#include "ichflysettings.h"
#include "main.h"

#ifdef arm9advsound
extern "C" int SPtoload;
extern "C" int SPtemp;

 __attribute__((section(".itcm")))void arm7dmareq()
{
	//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
	int oldIME = enterCriticalSection();
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //nothing here move along
	{
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		int i = 0;
		u32* src = (u32*)REG_IPC_FIFO_RX;
		//iprintf("i %08X\r\n",src);
		if(src != (u32*)0xFFFFFFFF)
		{
			//iprintf("%08X\r\n",REG_IPC_FIFO_RX);
			//iprintf("%08X %08X\n\r",src,REG_IPC_FIFO_CR);
			while(i < 4)
			{
				REG_IPC_FIFO_TX = *src;
				src+=4;
				i++;
			}
		}
		else
		{
			iprintf("i %08X\r\n",REG_IPC_FIFO_RX);
		}
	}
	leaveCriticalSection(oldIME);

	//DMA0_DEST = REG_IPC_FIFO_RX;
	//DMA0_CR = REG_IPC_FIFO_RX;
}
#else
void arm7dmareq()
{
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //nothing here move along
	{
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		//iprintf("in");
		VblankHandler();
		u32 src = REG_IPC_FIFO_RX; //send ack
		while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))src = REG_IPC_FIFO_RX;
	}
}
#endif
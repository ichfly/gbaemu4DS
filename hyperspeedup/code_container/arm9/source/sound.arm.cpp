#include <nds.h>
#include <stdio.h>
#include "ichflysettings.h"
#include "main.h"

#include "Cheats.h"

#ifdef arm9advsound
extern "C" int SPtoload;
extern "C" int SPtemp;

int cheatsCheckKeys();

int counttrans = 0;

int dasistnurzumtesten = 0;

extern u32 arm7amr9buffer;

void arm7dmareq()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//counttrans++;
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		int i = 0;
		u32* src = (u32*)REG_IPC_FIFO_RX;
		if(src < (u32*)0x10000000)
		{
			//iprintf("%08X\r\n",REG_IPC_FIFO_RX);
			//iprintf("%08X %08X\n\r",src,REG_IPC_FIFO_CR);
			if(counttrans < (u32)src)counttrans = (u32)src;
			while(i < 4)
			{
				REG_IPC_FIFO_TX = *src;
				//REG_IPC_FIFO_TX = 0;
				src++;
				i++;
			}
		}
		else
		{
			if(src == (u32*)0x3F00BEEF)
			{
				VblankHandler();
				dasistnurzumtesten++;
				if(dasistnurzumtesten == 60)
				{
					iprintf("alive %08X\r\n",counttrans);
					dasistnurzumtesten = 0;
					counttrans = 0;
				}
				//counttrans = 0;
			}
			if(src == (u32*)0x4000BEEF)
			{
				while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
				iprintf("arm7 %08X\r\n",REG_IPC_FIFO_RX);
			}
		}
		//iprintf("e %08X\r\n",REG_IPC_FIFO_CR);
	}

 }
void arm7dmareqandcheat()
{
#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		int i = 0;
		u32* src = (u32*)REG_IPC_FIFO_RX;
		//iprintf("i %08X\r\n",src);
		if(src < (u32*)0x10000000)
		{
			//iprintf("%08X\r\n",REG_IPC_FIFO_RX);
			//iprintf("%08X %08X\n\r",src,REG_IPC_FIFO_CR);
			while(i < 4)
			{
				REG_IPC_FIFO_TX = *src;
				src++;
				i++;
			}
		}
		else
		{
			if(src == (u32*)0x3F00BEEF)
			{
				cheatsCheckKeys();
				VblankHandler();
			}
		}
		//if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))arm7dmareqandcheat();
	}

 }
#else
void arm7dmareq()
{
	#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //nothing here move along
	{
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		//iprintf("in");
		VblankHandler();
		u32 src = REG_IPC_FIFO_RX; //send ack
		while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))src = REG_IPC_FIFO_RX;
	}
}
void arm7dmareqandcheat()
{
	#ifdef advanced_irq_check
	REG_IF = IRQ_FIFO_NOT_EMPTY;
#endif
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //nothing here move along
	{
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		//iprintf("in");
		cheatsCheckKeys();
		VblankHandler();
		u32 src = REG_IPC_FIFO_RX; //send ack
		while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))src = REG_IPC_FIFO_RX;
	}
}
#endif 
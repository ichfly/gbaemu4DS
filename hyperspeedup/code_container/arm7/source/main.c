/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <nds/arm7/audio.h>

volatile bool exitflag = false;
u16 callline = 0xFFFF;

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	ledBlink(0);
	readUserSettings();
	
	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();

	REG_IPC_SYNC = 0;
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE;
	REG_IPC_FIFO_CR = IPC_FIFO_SEND_CLEAR | IPC_FIFO_ENABLE | IPC_FIFO_ERROR;
	//enableSound();

	while (true) {
		if((REG_VCOUNT == callline) && (REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY))
		{
			REG_IPC_FIFO_TX = 0; //send cmd 0
			while(REG_VCOUNT == callline); //don't send 2 or more
		}
		if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
		{
			int addr = (REG_IPC_FIFO_RX & ~0xC0000000/*todo*/); //addr + flags //flags 2 most upperen Bits dma = 0 u8 = 1 u16 = 2 u32 = 3
			while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
			int val = (REG_IPC_FIFO_RX &  ~0x80000000); //Value skip add for speedup
			switch(addr)
			{
			case 0x1FFFFFFF:
			callline = val;
			default:
			break;
			}
		}
	}
	return 0;
}
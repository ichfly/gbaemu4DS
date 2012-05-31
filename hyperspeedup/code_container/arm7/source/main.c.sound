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
//#include <dswifi7.h> //ichfly later maybe
/*#include <maxmod7.h>





//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	Wifi_Update();
}


//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}


//---------------------------------------------------------------------------------
void powerButtonCB() {
//---------------------------------------------------------------------------------
	exitflag = true;
}
*/

	u8* soundbuffA = 0;
	u8* soundbuffB = 0;

volatile bool exitflag = false;

u8 dmaApart = 0;
u8 dmaBpart = 0;

u32 dmabuffer = 0;

u16 SOUNDCNT_L = 0;
u16 SOUNDCNT_H = 0;

u8 tacktgeber_sound_FIFO_DMA_A = 0;
u8 tacktgeber_sound_FIFO_DMA_B = 0;
u16 TM0CNT_L = 0;
u16 TM1CNT_L = 0;

u16 TM0CNT_H = 0;
u16 TM1CNT_H = 0;

u16 DMA1CNT_H = 0;
u16 DMA2CNT_H = 0;


u16 DMA1SAD_L = 0;
u16 DMA1SAD_H  = 0;
u16 DMA1DAD_L = 0;
u16 DMA1DAD_H  = 0;

u16 DMA2SAD_L = 0;
u16 DMA2SAD_H  = 0;
u16 DMA2DAD_L = 0;
u16 DMA2DAD_H  = 0;


//debug stuff

u32 debugsrc1 = 0;
u32 debugsrc2  = 0;
u32 debugfr1 = 0;
u32 debugfr2  = 0;


void dmaAtimerinter()
{
	REG_IPC_FIFO_TX = debugsrc1;
	//REG_IPC_SYNC |= IPC_SYNC_IRQ_REQUEST;
	//REG_IPC_FIFO_TX = 0x4000188;
	//REG_IPC_FIFO_TX = 0x8420004;
	debugsrc1+=16;
	int i = 0;
	while(i < 4)
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		*(u32*)(soundbuffA + i*4 + 0x10 * dmaApart) = REG_IPC_FIFO_RX;
		i++;
	}
	if(dmaApart == 0) dmaApart = 1;
	else dmaApart = 0;
}
void dmaBtimerinter()
{
	REG_IPC_FIFO_TX = debugsrc2;
	//REG_IPC_SYNC |= IPC_SYNC_IRQ_REQUEST;
	//REG_IPC_FIFO_TX = 0x4000188;
	//REG_IPC_FIFO_TX = 0x8420004;
	debugsrc2+=16;
	int i = 0;
	while(i < 4)
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		*(u32*)(soundbuffB + i*4 + 0x10 * dmaBpart) = REG_IPC_FIFO_RX;
		i++;
	}
	if(dmaBpart == 0) dmaBpart = 1;
	else dmaBpart = 0;
}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	ledBlink(0);
	readUserSettings();
	
	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();

	enableSound();
	//fifoInit();

	//mmInstall(FIFO_MAXMOD);

	//SetYtrigger(80);

	//installWifiFIFO();
	//installSoundFIFO();

	//installSystemFIFO();

	//irqSet(IRQ_VCOUNT, VcountHandler);
	//irqSet(IRQ_VBLANK, VblankHandler);

	//irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);
	
	//irqEnable(IRQ_VCOUNT);

	//setPowerButtonCB(powerButtonCB);   

	/*while(true) //test
	{
		if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))REG_IPC_FIFO_TX = REG_IPC_FIFO_RX + 1;
	}*/
	// Keep the ARM7 mostly idle

	REG_IPC_FIFO_CR = IPC_FIFO_SEND_CLEAR;
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE;


	soundbuffA = malloc(32);
	soundbuffB = malloc(32);


	SCHANNEL_SOURCE(4) = soundbuffA;
	SCHANNEL_REPEAT_POINT(4) = 32;
	SCHANNEL_LENGTH(4) = 0;

	SCHANNEL_SOURCE(5) = soundbuffB;
	SCHANNEL_REPEAT_POINT(5) = 32;
	SCHANNEL_LENGTH(5) = 0;

				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));



	//REG_IPC_FIFO_TX = 0x2000000;
	//REG_IPC_SYNC |= IPC_SYNC_IRQ_REQUEST;

	while (true) {
		//sound alloc
		//0-3 matching gba
		//4-5 FIFO
		//ledBlink(1);
		//swiWaitForVBlank();
		if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
		{
			int addr = (REG_IPC_FIFO_RX & ~0xC0000000/*todo*/); //addr + flags //flags 2 most upperen Bits dma = 0 u8 = 1 u16 = 2 u32 = 3
			while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
			int val = (REG_IPC_FIFO_RX &  ~0x80000000); //Value skip add for speedup
			//REG_IPC_FIFO_TX = 0;
			switch(addr)
			{
			  case 0:
/*REG_IPC_FIFO_TX = DMA1SAD_L;
REG_IPC_FIFO_TX = DMA1SAD_H;
REG_IPC_FIFO_TX = DMA1DAD_L;
REG_IPC_FIFO_TX = DMA1DAD_H;

REG_IPC_FIFO_TX = DMA2SAD_L;
REG_IPC_FIFO_TX = DMA2SAD_H;
REG_IPC_FIFO_TX = DMA2DAD_L;
REG_IPC_FIFO_TX = DMA2DAD_H;*/
				//REG_IPC_FIFO_TX = SCHANNEL_CR(4);
				//REG_IPC_FIFO_TX = debugsrc1;
				//REG_IPC_FIFO_TX = debugfr1;
				    REG_IPC_FIFO_TX = 0xFFFFFFFF;
					REG_IPC_FIFO_TX = val;
				  break;
			  case 0x4:
				  dmabuffer = val;
			  case 0xBC:
				DMA1SAD_L = val;
				break;
			  case 0xBE:
				DMA1SAD_H = val & 0x0FFF;
				break;
			  case 0xC0:
				DMA1DAD_L = val;
				checkstart();
				break;
			  case 0xC2:
				DMA1DAD_H = val & 0x07FF;
				checkstart();
				break;
			  case 0xC4:
				//DMA1CNT_L = val & 0x3FFF;
				break;
			  case 0xC6:
				DMA1CNT_H = val;
				checkstart();
				break;
			  case 0xC8:
				DMA2SAD_L = val;
				break;
			  case 0xCA:
				DMA2SAD_H = val & 0x0FFF;
				break;
			  case 0xCC:
				DMA2DAD_L = val;
				checkstart();
				break;
			  case 0xCE:
				DMA2DAD_H = val & 0x07FF;
				checkstart();
			  case 0xD0:
				//DMA2CNT_L = val & 0x3FFF;
				break;
			  case 0xD2:
				DMA2CNT_H = val;
				checkstart();
				break;





			case 0x100:
				TM0CNT_L = val;
				updatetakt();
			case 0x102:
				TM0CNT_H = val;
				updatetakt();
				checkstart();
			case 0x104:
				TM1CNT_L = val;
				updatetakt();
			case 0x106:
				TM1CNT_H = val;
				updatetakt();
				checkstart();
			case 0x80:
				SOUNDCNT_L = val;
				updatevol();
				break;
			case 0x82:
				SOUNDCNT_H = val; //Reset FIFO is not needed because we don't have direct streaming yet so don't need that
				updatevol();

				if(val & BIT(10))
				{
					tacktgeber_sound_FIFO_DMA_A = 1;
				}
				else
				{
					tacktgeber_sound_FIFO_DMA_A = 0;
				}
				if(val & BIT(14))
				{
					tacktgeber_sound_FIFO_DMA_B = 1;
				}
				else
				{
					tacktgeber_sound_FIFO_DMA_B = 0;
				}
				updatetakt();
				checkstart();
				break;
			case 0x84:
				if(val & 0x80)REG_SOUNDCNT |= 0x8000;
				else
				{
					REG_SOUNDCNT &= ~0x8000;
				}
				break;
			case 0x88:
					  //Amplitude Resolution/Sampling Cycle is not supported so only Bias
					  //it is better on the DS any way
				  REG_SOUNDBIAS = val;
				  break;
			default:
				break; //nothing
			}
		}
	}
	return 0;
}

void updatevol()
{

	/*****************************************************/
	/*                  Update Voll                      */
	/*****************************************************/


	//Sound_chan = (Volume_Right * enabeled + Volume_Left * enabeled) * (Soundcnt(1,2,4))*static_for_max
	//DMA = (Soundcnt(1,2) * enabeled + Soundcnt(1,2) * enabeled) * sataic_for_max
	SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF) | ((( 1 + ((SOUNDCNT_H & 0x4) >> 2))*((SOUNDCNT_H & BIT(8)) >> 8) + ( 1 + ((SOUNDCNT_H & 0x4) >> 2))*((SOUNDCNT_H & BIT(9)) >> 9))*31);     //max:124
	SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF) | ((( 1 + ((SOUNDCNT_H & 0x8) >> 3))*((SOUNDCNT_H & BIT(12)) >> 12) + ( 1 + ((SOUNDCNT_H & 0x8) >> 3))*((SOUNDCNT_H & BIT(13)) >> 13))*31); //max:124
	int Vol = SOUNDCNT_H & 0x3;
	switch(Vol)
	{
	case 3:
	case 0:
		Vol = 1;
		break;
	case 1:
		Vol = 2;
		break;
	case 2:
		Vol = 4;
		break;
	}
	int Masterright = SOUNDCNT_L & 0x7;
	int Masterleft =  (SOUNDCNT_L << 4) & 0x7;
	SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(8)) >> 8) + Masterleft * ((SOUNDCNT_L & BIT(12)) >> 12) ) * Vol *2);  //max:112
	SCHANNEL_CR(1) = (SCHANNEL_CR(1) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(9)) >> 9) + Masterleft * ((SOUNDCNT_L & BIT(13)) >> 13) ) * Vol *2);  //max:112
	SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(10)) >> 10) + Masterleft * ((SOUNDCNT_L & BIT(14)) >> 14) ) * Vol *2);//max:112
	SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(11)) >> 11) + Masterleft * ((SOUNDCNT_L & BIT(15)) >> 15) ) * Vol *2);//max:112


    /*****************************************************/
	/*                  Update Panning                   */
	/*****************************************************/
	

	if(SOUNDCNT_H & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000);
	if(SOUNDCNT_H & BIT(8)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_H & BIT(8) && SOUNDCNT_H & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x400000; //same on both

	if(SOUNDCNT_H & BIT(13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000);
	if(SOUNDCNT_H & BIT(12)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_H & BIT(12) && SOUNDCNT_H & BIT(13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x400000; //same on both

	int right = SOUNDCNT_L & 7;
	int left = (SOUNDCNT_L << 4) & 7;
	int tempmixedvol1_4 = 0;
	if((left + right) != 0) //don't work
	{
		tempmixedvol1_4 = ((right*0x7F0000)/(right + left) & 0x7F0000);
	}
	
	if(SOUNDCNT_L & BIT(12)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(8)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(8) && SOUNDCNT_L & BIT(12)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000) | tempmixedvol1_4;

	if(SOUNDCNT_L & BIT(13)) SCHANNEL_CR(4) = (SCHANNEL_CR(1) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(1) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(9) && SOUNDCNT_L & BIT(13)) SCHANNEL_CR(1) = (SCHANNEL_CR(1) & ~0xFF0000) | tempmixedvol1_4; 

	if(SOUNDCNT_L & BIT(14)) SCHANNEL_CR(4) = (SCHANNEL_CR(2) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(10)) SCHANNEL_CR(4) = (SCHANNEL_CR(2) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(10) && SOUNDCNT_L & BIT(14)) SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF0000) | tempmixedvol1_4;

	if(SOUNDCNT_L & BIT(15)) SCHANNEL_CR(4) = (SCHANNEL_CR(3) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(11)) SCHANNEL_CR(4) = (SCHANNEL_CR(3) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(11) && SOUNDCNT_L & BIT(15)) SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF0000) | tempmixedvol1_4;

}
void updatetakt()
{
	//FIFO A
	if(tacktgeber_sound_FIFO_DMA_A == 0)
	{
		int seek;
		switch(TM0CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(4) = debugfr1 = (((-TM0CNT_L) << seek) & 0xFFFF) >> 3;
	}
	else
	{
		int seek;
		switch(TM1CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(4) = debugfr1 = (((-TM1CNT_L) & 0xFFFF) << seek);
	}
	//FIFO B
	if(tacktgeber_sound_FIFO_DMA_B == 0)
	{
		int seek;
		switch(TM0CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(5) = debugfr2 = (((-TM0CNT_L) << seek) & 0xFFFF) >> 3;
	}
	else
	{
		int seek;
		switch(TM1CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(5) = debugfr2 = (((-TM1CNT_L) << seek) & 0xFFFF) >> 3;


	}
}
void checkstart()
{
	//DMA 1
	if(DMA1DAD_L == 0x00A0 && DMA1DAD_H == 0x0400 && (DMA1CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(TM0CNT_H & 0x80) //started timer
			{
				dmaAtimerinter();
				dmaAtimerinter();
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA1SAD_L | (DMA1SAD_H << 16);
				SCHANNEL_CR(4) |= 0x80000000; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 5, dmaAtimerinter);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 32;
				SCHANNEL_LENGTH(4) = 0;
			}
		}
		else
		{
			if(TM1CNT_H & 0x80)
			{
				dmaAtimerinter();
				dmaAtimerinter();
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA1SAD_L | (DMA1SAD_H << 16);
				SCHANNEL_CR(4) |= 0x80000000; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 5, dmaAtimerinter);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 32;
				SCHANNEL_LENGTH(4) = 0;
			}
		}
	}
	else
	{
		if(DMA1DAD_L == 0x00A4 && DMA1DAD_H == 0x0400 && (DMA1CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(TM0CNT_H & 0x80) //started timer
				{
					dmaBtimerinter();
					dmaBtimerinter();
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA1SAD_L | (DMA1SAD_H << 16);
					SCHANNEL_CR(5) |= 0x80000000; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 5, dmaBtimerinter);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 32;
					SCHANNEL_LENGTH(5) = 0;
				}
			}
			else
			{
				if(TM1CNT_H & 0x80)
				{
					dmaBtimerinter();
					dmaBtimerinter();
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA1SAD_L | (DMA1SAD_H << 16);
					SCHANNEL_CR(5) |= 0x80000000; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 5, dmaBtimerinter);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 32;
					SCHANNEL_LENGTH(5) = 0;
				}
			}
		}

	}


	//DMA 2
	if(DMA2DAD_L == 0x00A0 && DMA2DAD_H == 0x0400 && (DMA2CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(TM0CNT_H & 0x80) //started timer
			{
				dmaAtimerinter();
				dmaAtimerinter();
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA2SAD_L | (DMA2SAD_H << 16);
				SCHANNEL_CR(4) |= 0x80000000; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 5, dmaAtimerinter);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 32;
				SCHANNEL_LENGTH(4) = 0;
			}
		}
		else
		{
			if(TM1CNT_H & 0x80)
			{
				dmaAtimerinter();
				dmaAtimerinter();
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA2SAD_L | (DMA2SAD_H << 16);
				SCHANNEL_CR(4) |= 0x80000000; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 5, dmaAtimerinter);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 32;
				SCHANNEL_LENGTH(4) = 0;
			}
		}
	}
	else
	{
		if(DMA2DAD_L == 0x00A4 && DMA2DAD_H == 0x0400 && (DMA2CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(TM0CNT_H & 0x80) //started timer
				{
					dmaBtimerinter();
					dmaBtimerinter();
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA2SAD_L | (DMA2SAD_H << 16);
					SCHANNEL_CR(5) |= 0x80000000; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 5, dmaBtimerinter);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 32;
					SCHANNEL_LENGTH(5) = 0;
				}
			}
			else
			{
				if(TM1CNT_H & 0x80)
				{
					dmaBtimerinter();
					dmaBtimerinter();
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA2SAD_L | (DMA2SAD_H << 16);
					SCHANNEL_CR(5) |= 0x80000000; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 5, dmaBtimerinter);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 32;
					SCHANNEL_LENGTH(5) = 0;
				}
			}
		}

	}



}

#include <nds.h>
#include <stdio.h>

char temp[0x100000]; //tempbuff

#include <sound_bin.h>

void swiWaitForVBlank()
{
	if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK));
	iprintf("%s",temp);
	temp[0] = 0;
	while(!(REG_DISPSTAT & DISP_IN_VBLANK));
}



//u8 test[0x10] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x60,0x50,0x40,0x30,0x20,0x10,0x00,0x00};
u8 test[0x10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
u8 test2[0x10] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x60,0x50,0x40,0x30,0x20,0x10,0x00,0x00};


u32* srctest;

bool teil = 0;
void arm7dmareq()
{
	REG_IF = IRQ_FIFO_NOT_EMPTY;
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)) //handel all cmds
	{
		//counttrans++;
		//iprintf("SPtoload %x sptemp %x\r\n",SPtoload,SPtemp);
		int i = 0;
		u32* src = (u32*)REG_IPC_FIFO_RX;
		src = srctest+=4;
		//if(teil) src = (u32*)test2;
		//teil = teil ? 0:1;
		//sprintf(temp,"%08X",src);
		if(src < (u32*)0x10000000)		
		{
			//iprintf("%08X\r\n",REG_IPC_FIFO_RX);
			//iprintf("%08X %08X\n\r",src,REG_IPC_FIFO_CR);
			REG_IPC_FIFO_TX = 0x1;
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
				sprintf(temp,"test");
				continue;
			}
			if(src == (u32*)0x4000BEEF)
			{
				while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
				sprintf(temp,"arm7 %08X\r\n",REG_IPC_FIFO_RX);
				continue;
			}
			sprintf(temp,"error rec %08X %08X\r\n",src,REG_IPC_FIFO_CR);
		}
		//iprintf("e %08X\r\n",REG_IPC_FIFO_CR);
	}
 }



//---------------------------------------------------------------------------------
u32 arm7amr9buffer = 0;

int main( int argc, char **argv) 
{
	 videoSetModeSub(MODE_5_2D);


	//defaultExceptionHandler();	//for debug befor gbainit

	//set the first two banks as background memory and the third as sub background memory
	//D is not used..if you need a bigger background then you will need to map
	//more vram banks consecutivly (VRAM A-D are all 0x20000 bytes in size)
	vramSetPrimaryBanks(	VRAM_A_MAIN_BG_0x06000000/*for gba*/, VRAM_B_LCD, 
		VRAM_C_SUB_BG_0x06200000 , /*VRAM_D_LCD*/ VRAM_D_MAIN_BG_0x06020000 /*for BG emulation*/); //needed for main emulator

	vramSetBanks_EFG(VRAM_E_MAIN_SPRITE/*for gba sprite*/,VRAM_F_LCD/*cant use*/,VRAM_G_LCD/*cant use*/);
	vramSetBankH(VRAM_H_SUB_BG); //only sub /*for prints to lowern screan*/ 
	vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub

	srctest = (u32*)sound_bin;

	consoleDemoInit();

	iprintf("test1");
	REG_IPC_FIFO_TX = 0x1FFFFFFB;//must be ignored
	REG_IPC_FIFO_TX = 0x0;
	swiWaitForVBlank();
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
	{
		iprintf("error -1");
		while(1);
	}
	iprintf("test2");
	REG_IPC_FIFO_TX = 0x1FFFFFFC; //set on
	REG_IPC_FIFO_TX = 0x0;

	swiWaitForVBlank();

	REG_IPC_FIFO_TX = 0x1FFFFFFB;//must not be ignored
	REG_IPC_FIFO_TX = 0x0;
	swiWaitForVBlank();
	if(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)
	{
		iprintf("error -2");
		while(1);
	}
	if(REG_IPC_FIFO_RX != 0x4100BEEF)
	{
		iprintf("error -3");
		while(1);
	}
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
	{
		iprintf("error -4");
		while(1);
	}
	iprintf("test3");
	//set buffer
	REG_IPC_FIFO_TX = 0x1FFFFFFA;
	REG_IPC_FIFO_TX = arm7amr9buffer = (u32)malloc(0x100); //buffer for arm7

	irqSet(IRQ_FIFO_NOT_EMPTY,arm7dmareq); //async reader
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	iprintf("test4");
	REG_IPC_FIFO_TX = 0x1FFFFFFF; //Vsync
	REG_IPC_FIFO_TX = 0x159;

	REG_IPC_FIFO_TX = 0x82; //vol
	REG_IPC_FIFO_TX = 0x0304;

	REG_IPC_FIFO_TX = 0xBC; //src
	REG_IPC_FIFO_TX = 0x0001;
	REG_IPC_FIFO_TX = 0xBE;
	REG_IPC_FIFO_TX = 0x0001;

	swiWaitForVBlank();


	REG_IPC_FIFO_TX = 0xC0; //dest
	REG_IPC_FIFO_TX = 0x00A0;
	REG_IPC_FIFO_TX = 0xC2;
	REG_IPC_FIFO_TX = 0x0400;

	swiWaitForVBlank();


	REG_IPC_FIFO_TX = 0x100; //timer
	REG_IPC_FIFO_TX = 0xfc64;
	REG_IPC_FIFO_TX = 0x102;
	REG_IPC_FIFO_TX = 0x80;

	swiWaitForVBlank();


	REG_IPC_FIFO_TX = 0x84; //sound on
	REG_IPC_FIFO_TX = 0x80;

	swiWaitForVBlank();

	REG_IPC_FIFO_TX = 0xC6; //dma start
	REG_IPC_FIFO_TX = 0x8000;

	iprintf("ende");
	while(1)swiWaitForVBlank();


}
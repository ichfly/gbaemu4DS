/*---------------------------------------------------------------------------------

	Basic template code for starting a GBA app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include "../../common/gba_ipc.h"

//reload app
#include "./hbmenustub/nds_loader_arm9.h"


#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include <dirent.h>
#include "cpumg.h"
#include "bios.h"
#include "mydebuger.h"
#include "ds_dma.h"
#include "file_browse.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>    // for sbrk()

gbaHeader_t gbaheader;
u8 arm7exchangefild[0x100];

char* savetypeschar[7] =
	{(char*)"SaveTypeAutomatic",(char*)"SaveTypeEeprom",(char*)"SaveTypeSram",(char*)"SaveTypeFlash64KB",(char*)"SaveTypeEepromSensor",(char*)"SaveTypeNone",(char*)"SaveTypeFlash128KB"};

char* memoryWaitrealram[8] =
  { (char*)"10 and 6",(char*)"8 and 6",(char*)"6 and 6",(char*)"18 and 6",(char*)"10 and 4",(char*)"8 and 4",(char*)"6 and 4",(char*)"18 and 4" };


//---------------------------------------------------------------------------------
int main( int argc, char **argv) {

	ARMV5toARMV4Mode();	//so undefined resolver deals with proper armv4 opcodes and we iron out patches // for upcoming prefetch logic

	biosPath[0] = 0;
	savePath[0] = 0;
	patchPath[0] = 0;

	//reboot app
	bool isdsisdhardware=false;
	installBootStub(isdsisdhardware);

	//---------------------------------------------------------------------------------
	//set the mode for 2 text layers and two extended background layers
	//videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);	
	
	vramSetPrimaryBanks(	
	VRAM_A_MAIN_BG_0x06000000,      //Mode0 Tile/Map mode	//Mode 1/2/3/4 special bitmap/rotscale modes
	VRAM_B_LCD, //6820000h-683FFFFh	getVRAMHeapStart(); here
	VRAM_C_SUB_BG_0x06200000,
	VRAM_D_MAIN_BG_0x06020000
	);
	vramSetBanks_EFG(VRAM_E_MAIN_SPRITE/*for gba sprite*/,VRAM_F_LCD/*cant use*/,VRAM_G_LCD/*cant use*/);
	vramSetBankH(VRAM_H_SUB_BG); //only sub /*for prints to lowern screan*/ 
	vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub


#ifdef advanced_irq_check
	irqSet(IRQ_HBLANK,HblankHandler); //todo async
#ifdef forceHBlankirqs
	irqEnable(IRQ_HBLANK);
#endif
#endif
	__irqSet(IRQ_FIFO_NOT_EMPTY,arm7dmareq,irqTable); //todo async
	irqEnable(IRQ_FIFO_NOT_EMPTY);

	//bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	consoleDemoInit();

	REG_POWERCNT &= ~((POWER_3D_CORE | POWER_MATRIX) & 0xFFFF);//powerOff(POWER_3D_CORE | POWER_MATRIX); //3D use power so that is not needed

	//consoleDemoInitsubsc();
	iprintf("gbaemu4DS for r4i gold (3DS) (r4ids.cn) by ichfly / Coto \n");
	//consoleDemoInit();
	//soundEnable(); //sound finaly
	//fifoSetDatamsgHandler(FIFO_USER_02, arm7debugMsgHandler, 0);

	DISPCNT  = 0x0080;

	//rootenabelde[2] = fatMountSimple  ("sd", &__io_dsisd); //DSi//sems to be inited by fatInitDefault
	//fatInitDefault();
	//nitroFSInit();
	bool temptest = true;
	#ifdef standalone
	if (0 != argc )
	{
		if(argc > 1)
		{
			if(argv[1][0] == '1')
			{
				temptest = false;
			}
		}
		if(argc > 2)
		{
			if(argv[2][0] == '1')
			{
				lcdSwap();
			}
		}
	}
	#else
			if(argv[5][0] == '1')
			{
				lcdSwap();
			}
	#endif
	if(!(_io_dldi_stub.friendlyName[0] == 0x52 && _io_dldi_stub.friendlyName[5] == 0x4E) && temptest)
	{
			iprintf("Warning: you try to run gbaemu DS on %s gbaemu may not work\n press A to continue and ignore this",_io_dldi_stub.friendlyName);
		while(1) {
			scanKeys();
			if (keysDown()&KEY_A) break;
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		}
	}
	else
	{	
		//antysoftreset
		/*if(*(u32*)((u8*)(IntrMain) + 0x18) == 0xE51FF004)
		{
			int oldIME = enterCriticalSection();
			*(u32*)((u8*)(IntrMain) + 0x18) = 0xE14F0000;
			*(u32*)((u8*)(IntrMain) + 0x1C) = 0xE92D5002;
			leaveCriticalSection(oldIME);


		  //send seed patch
		  //fifoSendValue32(FIFO_USER_01,0x40000000);
		  //fifoSendValue32(FIFO_USER_01,0xAF8CA2B8);

		}*/
	}
	//data protbuff
	#ifdef arm9advsound
	arm7arm9buffer = (u32)arm7exchangefild; 
	SendArm7Command((u32)0x1FFFFFFA, (u32)arm7exchangefild); //(1)load buffer , (2)buffer for arm7
	
	#endif
	//test

	/*REG_IPC_FIFO_TX = 0;
	while(true)
	{
		int i = REG_IPC_FIFO_RX;
		iprintf("%08X\r\n",i);
		REG_IPC_FIFO_TX = i;

	}*/

	/*	iprintf("\n%x %x %x",getHeapStart(),getHeapEnd(),getHeapLimit());
	malloc(0x4000);
	iprintf("\n%x %x %x",getHeapStart(),getHeapEnd(),getHeapLimit());
	while(1);*/ //test getHeapEnd() is the needed thing
	iprintf("Init Fat...");
	
	if(fatInitDefault()){
		iprintf("OK\n");
	}else{
		iprintf("failed\n");
		int i = 0;
		while(i< 300)
		{
			//swiWaitForVBlank();
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			i++;
		}
	}

	//irqInit();
	iprintf("\x1b[2J");
	//main menü
	//while(1)iprintf("o");

#ifdef standalone
	browseForFile("");
	
	int ausgewauhlt = 0;
	int myflashsize = 0x10000;
	u32 pressed = 0;
	do 
	{
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		scanKeys();
		pressed = keysDownRepeat();
	} while (pressed);


	while(1){
		iprintf("\x1b[2J");
		for(int i = 0; i < 7; i++)
		{
			if(i == ausgewauhlt) iprintf("->");
			else iprintf("  ");
			iprintf(savetypeschar[i]);
			iprintf("\n");
		}
		do 
		{
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			scanKeys();
			pressed = keysDownRepeat();
		} while (!pressed);
		
		if (pressed&KEY_A)
		{
			if(ausgewauhlt == 6)
			{
				myflashsize = 0x20000;
				cpuSaveType = 3;
			}
			else
			{
				cpuSaveType = ausgewauhlt;
			}
			break;
		}
		if (pressed&KEY_DOWN && ausgewauhlt != 6){ ausgewauhlt++;}
		if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--;}
	}
	
	while(1){
		iprintf("\x1b[2J");
		iprintf("fps 60/%i\n",frameskip + 1);
		//swiWaitForVBlank();
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A) break;
		if (isdaas&KEY_UP) frameskip++;
		if (isdaas&KEY_DOWN && frameskip != 0) frameskip--;
	}
	
	int syncline =159;
	while(1) 
	{
		iprintf("\x1b[2J");
		iprintf("Videosyncline %i\n",syncline);
		//swiWaitForVBlank();
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A) break;
		if (isdaas&KEY_UP) syncline++;
		if (isdaas&KEY_DOWN && syncline != 0) syncline--;
		if (isdaas&KEY_RIGHT) syncline+=10;
		if (isdaas&KEY_LEFT && syncline != 0) syncline-=10;
	}
	
	bool slow = false;
	iprintf("\x1b[2J");
	iprintf("press B for slow emuation A for normal\n");
	while(1) 
	{

		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A)
		{
			slow = false;
			break;
		}
		if(isdaas&KEY_B)
		{
			slow = true;
			break;
		}
	}
	#else
	strcpy(szFile,argv[1]);
	strcpy(savePath,argv[2]);
	strcpy(biosPath,argv[3]);
	strcpy(patchPath,argv[4]);
	if(argv[11][0] == '1')cpuIsMultiBoot = true;
	else cpuIsMultiBoot = false;
	int myflashsize = 0x10000;
	u32 ausgewauhlt = (u32)strtol(argv[6],NULL,16);
	if(ausgewauhlt == 6)
	{
		myflashsize = 0x20000;
		cpuSaveType = 3;
	}
	else
	{
		cpuSaveType = ausgewauhlt;
	}
	frameskip = (u32)strtol(argv[7],NULL,16);
	int syncline =(u32)strtol(argv[9],NULL,16);
	bool slow;
	if(argv[10][0] == '1')slow = true;
	else slow = false;
	
	//copy inverse
	if(slow)useMPUFast= false;
	else useMPUFast = true;
	
	if(argv[8][0] == '1')
	{
		SendArm7Command((u32)0x1FFFFFFC, 0);	//send cmd
	}
	#endif

	bool extraram =false; 
	//if(!REG_DSIMODE) extraram = ram_init(DETECT_RAM); 
	//extraram = true; //testtest

	initspeedupfelder();
	iprintf("\x1b[2J");

	parseDebug = true;

	//if(argc == 2) {
	//iprintf("%s",szFile);
	//while(1);
	bool failed = false;

	iprintf("CPULoadRom...");
	failed = !CPULoadRom(szFile,extraram);
	if(failed)
	{
		printf("failed");
		while(1);
	}
	iprintf("OK\n");
	iprintf("CPUInit\n");
	CPUInit(biosPath, useBios,extraram);

	iprintf("CPUReset\n");
	CPUReset();
	
	//Save start
	
	{
		myflashsize = 0x10000;
		u32 ausgewauhlt = (u32)strtol(argv[6],NULL,16);
		if(ausgewauhlt == 6){
			myflashsize = 0x20000;
			saveType = cpuSaveType = 3;
			flashSetSize(myflashsize);
		}
		else{
			saveType = cpuSaveType = ausgewauhlt;
		}
	}
		
	strcpy(savePath,argv[2]);
	
	if(savePath[0] != 0){
	
		FILE * frh = fopen(savePath,"r");
		if(!frh){	//Create new savefile
			iprintf("no savefile found, creating new one... \n");
			CPUWriteBatteryFile(savePath);
		}
		fclose(frh);
	
		if(CPUReadBatteryFile(savePath)){
			iprintf("CPUReadBatteryOK\n");
		}
		else{
			iprintf("CPUReadBatteryFAIL\n");
			int i = 0;
			while(i< 300){
				//swiWaitForVBlank();
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));
				i++;
			}
		}
	}
	if(cpuSaveType == 3){
		flashSetSize(myflashsize);
	}
	//Save end
	
	REG_IME = IME_DISABLE;
	
	iprintf("BIOS_RegisterRamReset\n");
	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu to write to the internalRAM
	BIOS_RegisterRamReset(0xFF);
	
	iprintf("arm7init\n");
	VblankHandler();
	
	REG_IME = IME_ENABLE;
	SendArm7Command((u32)0x1FFFFFFF, (u32)syncline);	//cmd
	
	//coto
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		u32 src = REG_IPC_FIFO_RX;
	}
	iprintf("irqinit\n");
	anytimejmpfilter = 0;
	
	//anytimejmp = (VoidFn)0x3007FFC; //bios import
	/*
	REG_IPC_FIFO_TX = 0; //test backcall
	REG_IPC_FIFO_TX = 0x1234567;
	if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
	*/
	iprintf("emulateedbiosstart\n");
	dmaCopy(rom,(void*)0x02000000,0x40000);
	emulateedbiosstart();
	/*
	REG_IPC_FIFO_TX = 0; //test backcall
	REG_IPC_FIFO_TX = 0x1111111;
	if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
    iprintf("ndsMode\n");

	ndsMode();
	/*
	REG_IPC_FIFO_TX = 0; //test backcall
	REG_IPC_FIFO_TX = 0x2222222;
	if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
    iprintf("gbaInit\n");

#ifdef capture_and_pars
	videoBgDisableSub(0);
	vramSetBankH(VRAM_H_LCD); //only sub
	vramSetBankI(VRAM_I_LCD); //only sub
	 int iback = bgInitSub(3, BgType_ExRotation, BgSize_B16_256x256, 0,0);

	//bgSetScale(3,0x111,0x133);
	//bgSetRotateScale(iback,0,0x111,0x133);
	//bgSetRotateScale(iback,0,0x0F0,0x0D5);
	bgSetRotateScale(iback,0,0x0F0,0x0D6);
	bgUpdate();
#endif

	gbaInit(useMPUFast);	
	iprintf("gbaMode2\n");
	gbaMode2();
	
	iprintf("jump to IMAGE @ EWRAM (%08X)\n\r",(unsigned int)(rom));
	cpu_ArmJumpforstackinit((u32)rom, 0);
	
	while(1==1){
	}
	return 0;
}
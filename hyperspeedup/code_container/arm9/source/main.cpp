/*---------------------------------------------------------------------------------

	Basic template code for starting a GBA app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

#include "../../gloabal/cpuglobal.h"

#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include <dirent.h>

#include "cpumg.h"
#include "GBAinline.h"
#include "bios.h"

#include "mydebuger.h"

#include "file_browse.h"

#define MAXPATHLEN 256 

#include <nds.h>

#include "arm7sound.h"

#include "main.h"


char biosPath[MAXPATHLEN * 2];

char patchPath[MAXPATHLEN * 2];

char savePath[MAXPATHLEN * 2];

char szFile[MAXPATHLEN * 2];

void initspeedupfelder();

typedef struct
{
	u32 entryPoint;
	u8 logo[156];
	char title[0xC];
	char gamecode[0x4];
	u16 makercode;
	u8 is96h;
	u8 unitcode;
	u8 devicecode;
	u8 unused[7];
	u8 version;
	u8 complement;
	u16 checksum;
} __attribute__ ((__packed__)) gbaHeader_t;

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

u8 arm7exchangefild[0x500];

#define INT_TABLE_SECTION __attribute__((section(".dtcm")))


extern struct IntTable irqTable[MAX_INTERRUPTS] INT_TABLE_SECTION;

extern "C" void __irqSet(u32 mask, IntFn handler, struct IntTable irqTable[] );


#include <nds/arm9/dldi.h>

// The only built in driver
extern DLDI_INTERFACE _io_dldi_stub;




void emulateedbiosstart();

extern volatile u16 DISPCNT;

extern void HblankHandler(void);


void downgreadcpu();




#define GBA_EWRAM ((void*)(0x02000000))

#include <nds/disc_io.h>
#include <dirent.h>





char* savetypeschar[7] =
	{"SaveTypeAutomatic","SaveTypeEeprom","SaveTypeSram","SaveTypeFlash64KB","SaveTypeEepromSensor","SaveTypeNone","SaveTypeFlash128KB"};

char* memoryWaitrealram[8] =
  { "10 and 6","8 and 6","6 and 6","18 and 6","10 and 4","8 and 4","6 and 4","18 and 4" };





extern "C" void IntrMain();


extern "C" void testasm(u32* feld);
extern "C" void cpu_SetCP15Cnt(u32 v);
extern "C" u32 cpu_GetCP15Cnt();
extern "C" u32 pu_Enable();




extern int frameskip;


u32 arm7amr9buffer = 0;


#ifdef anyarmcom
u32 amr7sendcom = 0;
u32 amr7senddma1 = 0;
u32 amr7senddma2 = 0;
u32 amr7recmuell = 0;
u32 amr7directrec = 0;
u32 amr7indirectrec = 0;
u32 amr7fehlerfeld[10];
#endif

//---------------------------------------------------------------------------------
int main( int argc, char **argv) {


	//set the first two banks as background memory and the third as sub background memory
	//D is not used..if you need a bigger background then you will need to map
	//more vram banks consecutivly (VRAM A-D are all 0x20000 bytes in size)
	vramSetPrimaryBanks(	VRAM_A_MAIN_BG_0x06000000/*for gba*/, VRAM_B_LCD, 
		VRAM_C_SUB_BG_0x06200000 , /*VRAM_D_LCD*/ VRAM_D_MAIN_BG_0x06020000 /*for BG emulation*/); //needed for main emulator

	vramSetBanks_EFG(VRAM_E_MAIN_SPRITE/*for gba sprite*/,VRAM_F_LCD/*cant use*/,VRAM_G_LCD/*cant use*/);
	vramSetBankH(VRAM_H_SUB_BG); //only sub /*for prints to lowern screan*/ 
	vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub
	videoSetModeSub(MODE_5_2D);
	consoleDemoInit();




  biosPath[0] = 0;
  savePath[0] = 0;
  patchPath[0] = 0;




//---------------------------------------------------------------------------------
	//set the mode for 2 text layers and two extended background layers
	//videoSetMode(MODE_5_2D); 



	if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
	while(!(REG_DISPSTAT & DISP_IN_VBLANK)); //wait till arm7 is up
	if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
	while(!(REG_DISPSTAT & DISP_IN_VBLANK));


	iprintf("gbaemu4DS for r4i gold (3DS) (r4ids.cn) by ichfly\nBuild " __DATE__ "\n" );
	//iprintf("test1");


#ifdef advanced_irq_check
	irqSet(IRQ_HBLANK,HblankHandler); //todo async
#ifdef forceHBlankirqs
	irqEnable(IRQ_HBLANK);
#endif
#endif
	__irqSet(IRQ_FIFO_NOT_EMPTY,arm7dmareq,irqTable); //todo async
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	//iprintf("test2");
	//while(1);


	//bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);

	REG_POWERCNT &= ~((POWER_3D_CORE | POWER_MATRIX) & 0xFFFF);//powerOff(POWER_3D_CORE | POWER_MATRIX); //3D use power so that is not needed

	//consoleDemoInitsubsc();
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
		u32 tempPMD = 0;
#ifndef arm9advsound
		tempPMD |= 0x2;
#else
		tempPMD |= 0x1;
#endif
		if(argv[5][0] == '1')
		{
			lcdSwap();
#ifdef capture_and_pars
			tempPMD |= 0x4;
#endif
		}
#ifdef capture_and_pars
		else
		{
			tempPMD |= 0x8;
		}
#else
		tempPMD |= 0xC;
#endif
REG_IPC_FIFO_TX = 0xDFFFFFF9;
REG_IPC_FIFO_TX = tempPMD;
#endif
if(!(_io_dldi_stub.friendlyName[0] == 0x52 && _io_dldi_stub.friendlyName[5] == 0x4E) && temptest)
{
		iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
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
#ifdef anyarmcom
*(u32*)arm7exchangefild = (u32)&amr7sendcom;
*(u32*)(arm7exchangefild + 4) = (u32)&amr7senddma1;
*(u32*)(arm7exchangefild + 8) = (u32)&amr7senddma2;
*(u32*)(arm7exchangefild + 12) = (u32)&amr7recmuell;
*(u32*)(arm7exchangefild + 16) = (u32)&amr7directrec;
*(u32*)(arm7exchangefild + 20) = (u32)&amr7indirectrec;
*(u32*)(arm7exchangefild + 24) = (u32)&amr7fehlerfeld[0];
#endif

extern const unsigned char s7Bitpoly[];
extern const unsigned char s15Bitpoly[];

*(u32*)(arm7exchangefild + 0x50) = (u32)&s7Bitpoly[0];
*(u32*)(arm7exchangefild + 0x54) = (u32)&s15Bitpoly[0];
*(u32*)(arm7exchangefild + 0x58) = (u32)&ioMem[0];
REG_IPC_FIFO_TX = 0xDFFFFFFA; //load buffer
REG_IPC_FIFO_TX = arm7amr9buffer = (u32)arm7exchangefild; //buffer for arm7
//while(1);
#endif
//test

	iprintf("Init Fat...");
    



    if(fatInitDefault()){
        iprintf("OK\n");
    }else{
        iprintf("failed\n");
		while(1);
    }

#ifdef standalone
	iprintf("\x1b[2J");
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


		while(1)
		{
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
			while(1) 
	{
		iprintf("\x1b[2J");
		iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
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
		iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
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
		u8 slow;
	iprintf("\x1b[2J");
	iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
	iprintf("press B for slow emuation A for normal\n");
	while(1) 
	{

		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A)
		{
			slow = 0;
			break;
		}
		if(isdaas&KEY_B)
		{
			slow = 1;
			break;
		}
	}
	iprintf("\x1b[2J");


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
	if(argv[10][0] == '1')slow = 1;
	else if(argv[10][0] == '2')slow = 2;
	else slow = 0;
	if(argv[8][0] == '1')
	{
#ifdef wifidebuger

	FILE* patchf = fopen("fat:/wifimodul.bin", "rb");
	int readed = fread((void*)0x02380000,1,0x80000,patchf);
	fclose(patchf);
	irqDisable(IRQ_FIFO_NOT_EMPTY);
	REG_IPC_FIFO_TX = 0xDFFFFFF8;//wifi startup cmd
	REG_IPC_FIFO_TX = 0x0;//wifi startup val
	printf("SEEDUP %X\r\n",readed);
	//patchf = fopen("fat:/wificon.bin", "rb");
	if(argv[12][0] == '1')//if(patchf)
	{
		/*u32 trap;
		while(readed)
		{
			readed = fread(&trap,1,1,patchf);
			while(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL);
			REG_IPC_FIFO_TX = trap;
		}
		fclose(patchf);*/ 
		//enmacaddr[6] Wifi_SetChannel gbanum nifispeed
		u32 k = 0;
		while(k < 9)
		{
			while(REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL);
			REG_IPC_FIFO_TX = (u32)strtol(argv[12 + k],NULL,16);;
			k++;
		}
	}
	//else
	//{
		while(true)
		{
			while((REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)); //wait for cmds
			u32 src = REG_IPC_FIFO_RX;
			if(src == 1)
			{
				printf("wifi init OK\r\n");
				break;
			}
			else
			if(src > 0x200)
			{
				printf("wifi init failed %08X\r\n",src);
				while(true); //hang
			}
			else
			if(src < 0x200)
			{
				printf("wifi msg %08X\r\n",src);
			}
			else
			{
				printf("msg error %08X\r\n",src);
				while(true); //hang
			}
			
		}
	//}
	irqEnable(IRQ_FIFO_NOT_EMPTY);
#endif
	//REG_IPC_FIFO_TX = 0xDFFFFFFC; //send cmd can't work
	//REG_IPC_FIFO_TX = 0;
}
#endif
	





	bool extraram =false; 

initspeedupfelder();




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
	  	//iprintf("Hello World2!");
		iprintf("CPUInit\n");
		CPUInit(biosPath, false,false);//CPUInit(biosPath, useBios,extraram); //todo

	  iprintf("CPUReset\n");
      CPUReset();
		  
	  if(savePath[0] != 0)
	  {
		  iprintf("CPUReadBatteryFile...");
		  if(CPUReadBatteryFile(savePath))
		  {
			iprintf("OK\n");
		  }
		  else
		  {
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
	  }
	if(cpuSaveType == 3)flashSetSize(myflashsize);
	gbaHeader_t *gbaGame;

	
	gbaGame = (gbaHeader_t*)rom;

	iprintf("BIOS_RegisterRamReset\n");

	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu to write to the internalRAM

	BIOS_RegisterRamReset(0xFF);

	pu_Enable();
	
	iprintf("dmaCopy\n");

	dmaCopy( (void*)rom,(void*)0x2000000, 0x40000);

	iprintf("arm7init\n");


	VblankHandler();
	REG_IPC_FIFO_TX = 0xDFFFFFFF; //cmd
	REG_IPC_FIFO_TX = syncline;
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))u32 src = REG_IPC_FIFO_RX;
	iprintf("irqinit\n");

	anytimejmpfilter = 0;
	
	iprintf("emulateedbiosstart\n");

	emulateedbiosstart();

    iprintf("ndsMode\n");

	ndsMode();

    iprintf("gbaInit\n");


#ifdef capture_and_pars
	videoBgDisableSub(0);
	vramSetBankH(VRAM_H_LCD); //only sub
	vramSetBankI(VRAM_I_LCD); //only sub
	int iback = bgInitSub(3, BgType_ExRotation, BgSize_B16_256x256, 0,0);

	bgSetRotateScale(iback,0,0x0F0,0x0D6);
	bgUpdate();
#endif

	gbaInit(slow);


#ifndef capture_and_pars
	iprintf("gbaMode2\n");
#endif
	REG_IME = IME_ENABLE;
	gbaMode2();
#ifndef capture_and_pars
	iprintf("jump to (%08X)\n\r",rom);
#endif

	//iprintf("\x1b[2J"); //reset (not working huh)
	//show_mem();

	cpu_ArmJumpforstackinit((u32)rom, 0);
	
	
	
  
  while(true);

		 
  return 0;

}
//a
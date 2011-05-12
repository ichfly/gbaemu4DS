/*---------------------------------------------------------------------------------

	Basic template code for starting a DS app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>



#include <filesystem.h>
#include "GBA.h"
#include "Sound.h"
#include "unzip.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include "ram.h"
#include <nds/disc_io.h>
#include <dirent.h>


int framenummer;

#define public

char szFile[2048];


char filename[2048];
char biosFileName[2048];
char captureDir[2048];
char saveDir[2048];
char batteryDir[2048];
struct EmulatedSystem emulator;

char* rootdirnames [3] = {"nitro:/","fat:/","sd:/"};
bool rootenabelde[3];

int ausgewauhlt = 0;

char* currentdir =  (char*)0;

int seite = 0;

int dirfeldsize = 0;
char** names; //stupid i know but i don't know a better way
u32* dirfeldsizes;

int bg = 0;

char* memoryWaitrealram[8] =
  { "10 and 6","8 and 6","6 and 6","18 and 6","10 and 4","8 and 4","6 and 4","18 and 4" };







void speedtest()
{	
	iprintf("\x1b[2J");
	iprintf("fps %d",framenummer);
	framenummer = 0;
}










bool dirfolder (char* folder)
{	
		DIR *pdir;
		struct dirent *pent;
		struct stat statbuf;

		pdir=opendir(folder);

		if (pdir){
			
			while ((pent=readdir(pdir))!=NULL) {
	    		stat(pent->d_name,&statbuf);
	    		if(strcmp(".", pent->d_name) == 0)
	        		continue;
				void* temp = malloc((dirfeldsize + 1)*4);
				memcpy( (void *)temp , (void *)dirfeldsizes , (dirfeldsize)*4);
				//iprintf ("%x\n", (u32)dirfeldsizes);
				if(dirfeldsize > 2) free(dirfeldsizes); //memory leak
				dirfeldsizes = (u32*)temp;
				char** temp2 = (char**)malloc((dirfeldsize + 1)*4);
				//iprintf ("%x\n", (u32)temp2);
				memcpy( (void *)temp2 , (void *)names , (dirfeldsize)*4);
				if(dirfeldsize > 2)free(names); //memory leak
				names = temp2;
				//iprintf ("%x\n", (u32)temp2);
				//iprintf ("%x\n", (u32)names);
	    		if(S_ISDIR(statbuf.st_mode))
				{
	        		*(u32*)(dirfeldsize + dirfeldsizes) = 0xFFFFFFFF;
					//iprintf("%s <dir>%d\n ", pent->d_name, statbuf.st_size);
				}
	    		if(!(S_ISDIR(statbuf.st_mode)))
				{
					*(u32*)(dirfeldsize + dirfeldsizes) = statbuf.st_size;
					//iprintf ("%d\n", *(u32*)(dirfeldsize + dirfeldsizes));
					//iprintf("%s %ld\n", pent->d_name, statbuf.st_size);
	        	}
				*(char**)((u32)names + dirfeldsize * 4) = (char*)malloc(strlen(pent->d_name) + 2);
				sprintf(*(char**)((u32)names + dirfeldsize * 4),"%s",pent->d_name);
				dirfeldsize++;
				//while(1);
				//iprintf ("%d\n", dirfeldsize);
				//free(temp2);
				//free(temp);
			}
			closedir(pdir);
		} else {
			//iprintf ("opendir() failure; terminating\n");
			return false;
		}
	/*for(int i = 0; i < dirfeldsize; i++)
	{
		//iprintf ("%d\n", *(u32*)((u32)dirfeldsizes + i * 4));
		iprintf ("%x\n", (char**)((u32)names + i * 4));
	}
	iprintf ("%d\n",dirfeldsize);*/
		
	return true;

}



void dirfree ()
{
	for(int i = 0; i < dirfeldsize - 1; i++)
	{
		//iprintf ("%x\n", *(u32*)((u32)names + i * 4));
		free(*(char**)((u32)names + i * 4));
	}
	//while(1);
	free(names);
	if(dirfeldsize > 2)free(dirfeldsizes); //malloc return 0x341 when i free that but why
	dirfeldsize = 0;
}



void display ()
{
	for(int i = ausgewauhlt - (ausgewauhlt%10); i < dirfeldsize && i < ausgewauhlt - (ausgewauhlt%10) + 10 ; i++)
	{
		if(i == ausgewauhlt)iprintf("->");
		else iprintf("  ");

		if(*(u32*)(i + dirfeldsizes) == 0xFFFFFFFF)iprintf("%s <dir>\n", *(char**)((u32)names + i * 4));
		else iprintf("%s (%ld)\n", *(char**)((u32)names + i * 4), *(u32*)(dirfeldsizes + i));
	}
}




int main(void) {
//---------------------------------------------------------------------------------
	//set the mode for 2 text layers and two extended background layers
	videoSetMode(MODE_5_2D); 

	//set the first two banks as background memory and the third as sub background memory
	//D is not used..if you need a bigger background then you will need to map
	//more vram banks consecutivly (VRAM A-D are all 0x20000 bytes in size)
	vramSetPrimaryBanks(	VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_SPRITE, 
		VRAM_C_SUB_BG , VRAM_D_LCD); 



	consoleDemoInit();

	
fatInitDefault();
nitroFSInit();


















	irqInit();
	
	
	irqEnable( IRQ_VBLANK);
	
	
	bool nichtausgewauhlt = true;
	
	iprintf("\x1b[2J");
//main menü
	while(nichtausgewauhlt)
	{
		for(int i = 0; i < 3; i++)
		{
			if(i == ausgewauhlt) printf("->");
			else printf("  ");
			printf(rootdirnames[i]);
			printf("\n");
		}
		while(nichtausgewauhlt)
		{
			swiWaitForVBlank();
			scanKeys();
			if (keysDown()&KEY_A)
			{
				int ausgewauhlt2 = ausgewauhlt;
				dirfolder(rootdirnames[ausgewauhlt2]);
				//menue start
				
				while(nichtausgewauhlt)
				{
					iprintf("\x1b[2J");
					display();
					while(nichtausgewauhlt)
					{
						swiWaitForVBlank();
						scanKeys();
						if (keysDown()&KEY_DOWN){ ausgewauhlt++; if(ausgewauhlt == dirfeldsize)ausgewauhlt = 0;  break;}
						if (keysDown()&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--; break;}
						if (keysDown()&KEY_A)
						{
							if(currentdir == (char*)0)
							{
								currentdir = (char*)malloc(strlen(rootdirnames[ausgewauhlt2]) + strlen(*(char**)((u32)names + ausgewauhlt * 4)) + 4);
								sprintf(currentdir,"%s%s",rootdirnames[ausgewauhlt2],*(char**)((u32)names + ausgewauhlt * 4));
							}
							else
							{
								char* currentdirtemp = (char*)malloc(strlen(currentdir) + strlen(*(char**)((u32)names + ausgewauhlt * 4)) + 4);
								sprintf(currentdirtemp,"%s/%s",currentdir,*(char**)((u32)names + ausgewauhlt * 4));								
								//free(currentdir);
								currentdir = currentdirtemp;
							}
							
							//iprintf("%d",(*(u32*)(dirfeldsizes + ausgewauhlt)));
							
							if(!(*(u32*)(dirfeldsizes + ausgewauhlt) == 0xFFFFFFFF))
							{
								sprintf(szFile,"%s",currentdir);
								free(currentdir);
								nichtausgewauhlt = false;
							}
							//iprintf(currentdir);
							dirfree();
							dirfolder(currentdir);
							ausgewauhlt = 0;
						}
					}
				}
				//menue ende
				//break;
			}
			if (keysDown()&KEY_DOWN && ausgewauhlt != 2){ ausgewauhlt++; break;}
			if (keysDown()&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--; break;}
		}
		iprintf("\x1b[2J");	
	}
	dirfree();

	/*iprintf(szFile);
	while(1);*/


	bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);


	//u16 colorMask = 0x1F;


	u16* backBuffer = (u16*)bgGetGfxPtr(bg);

		//draw a box (0,0,256,256)
		for(int iy = 0; iy < 256 - 0; iy++)
			for(int ix = 0; ix < 256 - 0; ix++) 
				backBuffer[iy * 256 + ix] = 0;//(rand() /*& colorMask*/) | BIT(15);
		//swap the back buffer to the current buffer
		backBuffer = (u16*)bgGetGfxPtr(bg);

	

//BgType_Bmp8
	while(1) {
		iprintf("gbaemu DS by ichfly\n");
		iprintf("frameskip %i\n",frameSkip);
		
		swiWaitForVBlank();
		iprintf("\x1b[2J");
		scanKeys();
		if (keysDown()&KEY_A) break;
		if (keysDown()&KEY_UP) frameSkip++;
		if (keysDown()&KEY_DOWN && frameSkip != 0) frameSkip--;
	}	
	
	bool extraram =false; 
	if(!REG_DSIMODE) extraram = ram_init(DETECT_RAM);
	if(extraram)
	{
		int clock = 0;
		while(1) 
		{
			iprintf("gbaemu DS by ichfly\n");
			iprintf("\nSlot-2 ram %s detected\nSize:%d\n",ram_type_string(),ram_size());
			iprintf("clock:%s\n\n",memoryWaitrealram[clock]);
		
			swiWaitForVBlank();
			iprintf("\x1b[2J");
			scanKeys();
			if (keysDown()&KEY_A) break;
			if (keysDown()&KEY_UP && clock != 7) clock++;
			if (keysDown()&KEY_DOWN && clock != 0) clock--;
			}
			REG_EXMEMCNT = (REG_EXMEMCNT | (clock << 2));
	}
	


  captureDir[0] = 0;
  saveDir[0] = 0;
  batteryDir[0] = 0;
  
  char buffer[1024];

  systemFrameSkip = frameSkip; 
  //gbBorderOn = 0;

  parseDebug = true;

  //if(argc == 2) {
  
    //char* szFile = "gba.gba";
    bool failed = false;
   
      failed = !CPULoadRom(szFile,extraram);
	  	//iprintf("Hello World2!");
		
       emulator = GBASystem;
		if(failed){ while(1);}
      CPUInit(biosFileName, useBios,extraram);
      CPUReset();
	  

	  if(batteryDir[0] != 0)CPUReadBatteryFile(batteryDir);
  /*} else {
    cartridgeType = 0;
    strcpy(filename, "gnu_stub");
    rom = (u8 *)malloc(0x2000000);
    workRAM = (u8 *)calloc(1, 0x40000);
    bios = (u8 *)calloc(1,0x4000);
    internalRAM = (u8 *)calloc(1,0x8000);
    paletteRAM = (u8 *)calloc(1,0x400);
    vram = (u8 *)calloc(1, 0x20000);
    oam = (u8 *)calloc(1, 0x400);
    pix = (u8 *)calloc(1, 4 * 241 * 162);
    ioMem = (u8 *)calloc(1, 0x400);

    emulator = GBASystem;
    
    CPUInit(biosFileName, useBios);
    CPUReset();    
  }*/
  
  timerStart(0, ClockDivider_1024,  TIMER_FREQ_1024(1),speedtest); // 1 sec

systemRedShift = 0;
systemGreenShift = 5;
systemBlueShift = 10;

utilUpdateSystemColorMaps();

int rrrresxfss = 0;

  //soundInit();
		
  while(true) {
     /* if(debugger && emulator.emuHasDebugger)
        dbgMain();
      else*/
        emulator.emuMain(emulator.emuCount);
	  	  	//iprintf("%d\r\n",rrrresxfss);
			
		//rrrresxfss++;
  }

		 
  return 0;

}
//b
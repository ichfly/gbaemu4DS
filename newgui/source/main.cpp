/*-----------------------------------------------------------------
 Copyright (C) 2005 - 2010
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Genera
 ,l Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <sys/stat.h>

#include <string.h>
#include <unistd.h>


#include "nds_loader_arm9.h"




//common

#define MAX_PATH 512
#define MAX_FILE 512




#include <string>
#include <vector>

#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <sys/stat.h>

#include <string.h>
#include <unistd.h>

#include "nds_loader_arm9.h"

#include "hbmenu_banner.h"


#include <nds/arm9/dldi.h>
extern DLDI_INTERFACE _io_dldi_stub;

char biosPath[MAXPATHLEN * 2];

char patchPath[MAXPATHLEN * 2];

char savePath[MAXPATHLEN * 2];

char szFile[MAXPATHLEN * 2];

bool cpuIsMultiBoot = false;

char* arcvsave = (char*)0;

typedef struct
{
	u32 entryPoint;
	u8 logo[156];
	char title[0xC];
	char gamecode[0x4];
	char makercode[0x2];
	u8 is96h;
	u8 unitcode;
	u8 devicecode;
	u8 unused[7];
	u8 version;
	u8 complement;
	u16 res;
	u8 somedata[100000];
} __attribute__ ((__packed__)) gbaHeader_tf;


typedef struct
{
	u32 Version;
	u32 listentr;
} __attribute__ ((__packed__)) patch_t;

typedef struct
{
	u32 gamecode;
	u8 homebrew;
	u64 crc;
	char patchPath[MAXPATHLEN * 2];
	u8 swaplcd;
	u8 savfetype;
	u8 frameskip;
	u8 frameskipauto;
	u16 frameline;
	u8 fastpu;
	u8 mb;
	u8 loadertype;
} __attribute__ ((__packed__)) patch2_t;



gbaHeader_tf gbaheaderf;

patch_t patchheader;

char* readabelnameversionsschar[18] =
	{"oldirq","newirq","advirq","HblancDMA","forceHblanc",
	"newirqsound","advirqsound","HblancDMAsound","forceHblancsound",
	"newirqsoundsc","advirqsoundsc","HblancDMAsoundsc","forceHblancsoundsc",
	"oldirqsc","newirqsc","advirqsc","HblancDMAsc","forceHblancsc"};

char* pathversionschar[18] =
	{"fat:/GBADS/oldirq.loader","fat:/GBADS/newirq.loader","fat:/GBADS/advirq.loader","fat:/GBADS/HblancDMA.loader","fat:/GBADS/forceHblanc.loader",
	"fat:/GBADS/newirqsound.loader","fat:/GBADS/advirqsound.loader","fat:/GBADS/HblancDMAsound.loader","fat:/GBADS/forceHblancsound.loader",
	"fat:/GBADS/newirqsoundsc.loader","fat:/GBADS/advirqsoundsc.loader","fat:/GBADS/HblancDMAsoundsc.loader","fat:/GBADS/forceHblancsoundsc.loader",
	"fat:/GBADS/oldirqsc.loader","fat:/GBADS/newirqsc.loader","fat:/GBADS/advirqsc.loader","fat:/GBADS/HblancDMAsc.loader","fat:/GBADS/forceHblancsc.loader"};

u8 inputtoVersion[18] = 
	{
		6,2,10,14,0,1,3,4,5,7,8,9,11,12,13,15,16,17
	};

#define nummerVersions 17


char* savetypeschar[7] =
	{"SaveTypeAutomatic","SaveTypeEeprom","SaveTypeSram","SaveTypeFlash64KB","SaveTypeEepromSensor","SaveTypeNone","SaveTypeFlash128KB"};

char* listless = "fat:/GBADS/internal_list.list";

using namespace std;






vu32 power_flag;


u32 global_cycles_per_instruction = 1;
//u64 frame_count_initial_timestamp = 0;
//u64 last_frame_interval_timestamp;
u32 psp_fps_debug = 0;
u32 skip_next_frame_flag = 0;
//u32 frameskip_counter = 0;

u32 cpu_ticks = 0;
u32 frame_ticks = 0;

u32 execute_cycles = 960;
s32 video_count = 960;
//u32 ticks;

//u32 arm_frame = 0;
//u32 thumb_frame = 0;
u32 last_frame = 0;

u32 synchronize_flag = 1;

//const char main_path[]="mmc:\\NDSGBA";
char main_path[MAX_PATH];

//Removing rom_path due to confusion
//char rom_path[MAX_PATH];

//vu32 quit_flag;

vu32 real_frame_count = 0;
u32 virtual_frame_count = 0;
vu32 vblank_count = 0;
u32 num_skipped_frames = 0;
u32 frames;

unsigned int pen = 0;
unsigned int frame_interval = 60; // For in-memory saved states used in rewinding

int date_format= 2;

u32 prescale_table[] = { 0, 6, 8, 10 };

char *file_ext[] = { ".gba", ".bin", ".zip", NULL };


u32 sound_on = 0;
char gamepak_filename[MAX_FILE];


void quit()
{

#ifdef USE_DEBUG
	fclose(g_dbg_file);
#endif
	while (1) {
		swiWaitForVBlank();
	}
}

extern "C" void initich(int argc, char **argv);
extern "C" int gui_init(unsigned int);
extern "C" void initial_gpsp_config();
extern "C" void init_game_config();
extern "C" void init_input();
extern "C" u32 menu(u16 *screen, int FirstInvocation);


//---------------------------------------------------------------------------------
void stop (void) {
//---------------------------------------------------------------------------------
	quit();
}

//---------------------------------------------------------------------------------
void maino(int argc, char **argv) {
//---------------------------------------------------------------------------------
  initich(argc,argv);
  char load_filename[MAX_FILE];

    if(gui_init(0) < 0)
        exit(0);
  // Initial path information
  initial_gpsp_config();


  power_flag = 0;


#ifdef USE_DEBUG
  g_dbg_file = fopen(DBG_FILE_NAME, "awb");
  DBGOUT("\nStart gpSP\n");
#endif

  init_game_config();




  init_input();


  // ROM
  gamepak_filename[0] = 0;

  // BIOS
  //char bios_filename[MAX_FILE];
  //sprintf(bios_filename, "%s/%s", main_path, "gba_bios.bin");

#define GBA_SCREEN_WIDTH 240
#define GBA_SCREEN_HEIGHT 160
#define GBA_SCREEN_BUFF_SIZE GBA_SCREEN_WIDTH*GBA_SCREEN_HEIGHT


    u16 screen_copy[GBA_SCREEN_BUFF_SIZE];
    memset((char*)screen_copy, 0, sizeof(screen_copy));
    menu(screen_copy, 1 /* first invocation: yes */);








//ichfly
extern char ichgamefileplpath[MAX_FILE];
#define arcvsave     ichgamefileplpath
		char filePath[MAXPATHLEN * 2];


			vector<char*> argarray;

			argarray.push_back("a");
			strcpy(filePath,arcvsave);


			sprintf(savePath,"%s.sav",arcvsave);
			FILE *pFile2 = fopen(savePath, "r");
			if(pFile2==NULL)savePath[0] = 0;
			fclose(pFile2);
			
			sprintf(patchPath,"%s.pat",arcvsave);
			FILE *pFile3 = fopen(patchPath, "r");
			if(pFile3==NULL)patchPath[0] = 0;
			fclose(pFile3);

			
			argarray.push_back(filePath);
			argarray.push_back(savePath);
			argarray.push_back("\0");
			
			FILE *gbafile = fopen(arcvsave, "r");
			
			fread((char*)&gbaheaderf, 1, sizeof(gbaHeader_tf),gbafile);
			
			FILE *listfiledata = fopen(listless, "r");
			if(listfiledata==NULL)
			{
				iprintf("incorrect setup");
				stop();
			}
			

			fread((char*)&patchheader, 1, sizeof(patch_t),listfiledata);
			
			patch2_t* entries = (patch2_t*)malloc(sizeof(patch2_t)*patchheader.listentr);
			//iprintf("%08X %08X %08X %08X %08X",entries,sizeof(patch2_t),patchheader.listentr,sizeof(patch2_t)*patchheader.listentr,listfiledata);
			//while(1);
			
			fread((char*)entries, 1, sizeof(patch2_t)*patchheader.listentr,listfiledata);
			
			fclose(gbafile);
			fclose(listfiledata);

			

			u32 ourcrc = swiCRC16(0x0,(void*)&patchheader,sizeof(gbaHeader_tf)) + swiCRC16(0xFFFF,(void*)&patchheader,sizeof(gbaHeader_tf)) << 16;
			//checksettings
			u32 matching = 0;
			for(int i2 = 0;i2 < patchheader.listentr;i2++)
			{
				if(*(u32*)gbaheaderf.gamecode == entries[i2].gamecode)
				{
					if(!entries[i2].homebrew)
					{
						matching = i2;
						break;
					}
					else
					{
						if(ourcrc == entries[i2].crc)
						{
							matching = i2;
							break;
						}
					}
				}
			}
			
			//setsettings
			if(patchPath[0] == 0)
			{
					sprintf(patchPath,"%s",entries[matching].patchPath);
			} 
			
			
			argarray.push_back(patchPath);
			
			
			char temp1[MAXPATHLEN * 2];
			char temp2[MAXPATHLEN * 2];
			char temp3[MAXPATHLEN * 2];
			
			if(entries[matching].swaplcd)argarray.push_back("1");
			else argarray.push_back("0");
			
			sprintf(temp1,"%X",entries[matching].savfetype);
			argarray.push_back(temp1);
			
			sprintf(temp2,"%X",entries[matching].frameskip);
			argarray.push_back(temp2);
			
			if(entries[matching].frameskipauto)argarray.push_back("1");
			else argarray.push_back("0");
			
			sprintf(temp3,"%X",entries[matching].frameline);
			argarray.push_back(temp3);
			
			if(entries[matching].fastpu == 1)argarray.push_back("1");
			else if(entries[matching].fastpu == 2)argarray.push_back("2");
			else argarray.push_back("0");
			
			if(entries[matching].mb)argarray.push_back("1");
			else argarray.push_back("0");
			
			argarray.at(0) = pathversionschar[entries[matching].loadertype];
		
			//argarray.at(0) = filePath;
			//iprintf ("Running %s with %d parameters\n", argarray[0], argarray.size());
			//while(1);
			int err = runNdsFile (argarray[0], argarray.size(), (const char **)&argarray[0]);
			iprintf ("Start failed. Error %i\n", err);







  return;
}
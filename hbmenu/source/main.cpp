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
#include <fat.h>
#include <sys/stat.h>

#include <string.h>
#include <unistd.h>

#include "nds_loader_arm9.h"
#include "file_browse.h"

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


//---------------------------------------------------------------------------------
void stop (void) {
//---------------------------------------------------------------------------------
	while (1) {
		swiWaitForVBlank();
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------


	// install the default exception handler
    defaultExceptionHandler();

	biosPath[0] = 0;
	savePath[0] = 0;
	patchPath[0] = 0;
	// overwrite reboot stub identifier
	extern u64 *fake_heap_end;
	*fake_heap_end = 0;

	char filePath[MAXPATHLEN * 2];
	int pathLen;
	std::string filename;

	videoSetMode(MODE_5_2D);
	vramSetBankA(VRAM_A_MAIN_BG);

	// Subscreen as a console
	videoSetModeSub(MODE_0_2D);
	vramSetBankH(VRAM_H_SUB_BG);
	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

	if (!fatInitDefault()) {
		iprintf ("fatinitDefault failed!\n");
		stop();
	} 

	keysSetRepeat(25,5);
	
	vector<string> extensionList; //ndsfile,gbafile,savefile,biospath,patfile,swaplcd,savfetype,frameskip,frameskipauto,frameline,fastpu,mb
	//extensionList.push_back(".nds");
	//extensionList.push_back(".argv");
	extensionList.push_back(".gba");
	extensionList.push_back(".agb");
	extensionList.push_back(".bin");
	extensionList.push_back(".mb");
	extensionList.push_back(".sav");
	extensionList.push_back(".pat");

		vector<char*> argarray;
		if(argc != 2)
		{
			// set up our bitmap background
			bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
			
			decompress(hbmenu_bannerBitmap, BG_GFX,  LZ77Vram);

			//filename = browseForFile(extensionList);
			browseForFile(extensionList);
			if(arcvsave != (char*)0) goto dataluncher;
			argarray.push_back("a");
			argarray.push_back(szFile);
			argarray.push_back(savePath);
			argarray.push_back(biosPath);
			argarray.push_back(patchPath);
			//stop();
			
			// Construct a command line
			/*getcwd (filePath, MAXPATHLEN);
			pathLen = strlen (filePath);


			argarray.push_back(strdup(filename.c_str()));

			char *name = argarray.at(0);
			strcpy (filePath + pathLen, name);
			free(argarray.at(0));*/
			
			
						int pressed;

	iprintf("\x1b[2J");
	iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
	iprintf("press B for lcdswap A for normal\n");
	while(1) 
	{

		swiWaitForVBlank();
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A)
		{
			argarray.push_back("0");
			break;
		}
		if(isdaas&KEY_B)
		{
			argarray.push_back("1");
			break;
		}
	}
	
	
				char temp1[MAXPATHLEN * 2];
				char temp2[MAXPATHLEN * 2];
				char temp3[MAXPATHLEN * 2];
		int ausgewauhlt = 0;
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
				swiWaitForVBlank();
				scanKeys();
				pressed = keysDownRepeat();
			} while (!pressed);

			if (pressed&KEY_A)
			{
				sprintf(temp1,"%X",ausgewauhlt);
				argarray.push_back(temp1);
				break;
			}
			if (pressed&KEY_DOWN && ausgewauhlt != 6){ ausgewauhlt++;}
			if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--;}
		}
		int frameskip = 0;
		while(1) 
		{
			iprintf("\x1b[2J");
			iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
			iprintf("fps 60/%i\n",frameskip + 1);
			swiWaitForVBlank();
			scanKeys();
			int isdaas = keysDownRepeat();
			if (isdaas&KEY_A) break;
			if (isdaas&KEY_UP) frameskip++;
			if (isdaas&KEY_DOWN && frameskip != 0) frameskip--;
		}
				sprintf(temp2,"%X",frameskip);
				argarray.push_back(temp2);

	iprintf("\x1b[2J");
	iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
	iprintf("press A for autovsync B for normal\n");
	while(1) 
	{

		swiWaitForVBlank();
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A)
		{
			argarray.push_back("1");
			break;
		}
		if(isdaas&KEY_B)
		{
			argarray.push_back("0");
			break;
		}
	}

		int syncline =159;
		while(1) 
		{
			iprintf("\x1b[2J");
			iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
			iprintf("Videosyncline %i\n",syncline);
			swiWaitForVBlank();
			scanKeys();
			int isdaas = keysDownRepeat();
			if (isdaas&KEY_A) break;
			if (isdaas&KEY_UP) syncline++;
			if (isdaas&KEY_DOWN && syncline != 0) syncline--;
			if (isdaas&KEY_RIGHT) syncline+=10;
			if (isdaas&KEY_LEFT && syncline != 0) syncline-=10;
		}
		sprintf(temp3,"%X",syncline);
		argarray.push_back(temp3);


	iprintf("\x1b[2J");
	iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
	iprintf("press B for slow A for normal UP for speedhack\n");
	while(1) 
	{

		swiWaitForVBlank();
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A)
		{
			argarray.push_back("0");
			break;
		}
		if(isdaas&KEY_B)
		{
			argarray.push_back("1");
			break;
		}
		if (isdaas&KEY_UP)
		{
			argarray.push_back("2");
			break;
		}

	}
		iprintf("\x1b[2J");
	iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
	iprintf("press B for mb A for normal\n");
	while(1) 
	{

		swiWaitForVBlank();
		scanKeys();
		int isdaas = keysDownRepeat();
		if (isdaas&KEY_A)
		{
			argarray.push_back("0");
			break;
		}
		if(isdaas&KEY_B)
		{
			argarray.push_back("1");
			break;
		}
	}

			ausgewauhlt = 0;
			while(1)
			{
				iprintf("\x1b[2J");
				for(int i = 0; i < nummerVersions; i++)
				{
					if(i == ausgewauhlt) iprintf("->");
					else iprintf("  ");
					iprintf(readabelnameversionsschar[inputtoVersion[i]]);
					iprintf("\n");
				}
				do 
				{
					swiWaitForVBlank();
					scanKeys();
					pressed = keysDownRepeat();
				} while (!pressed);

				if (pressed&KEY_A)
				{
					argarray.at(0) = pathversionschar[inputtoVersion[ausgewauhlt]];
					break;
				}
				if (pressed&KEY_DOWN && ausgewauhlt < nummerVersions){ ausgewauhlt++;}
				if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--;}
			}
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
		}
		else
		{
			arcvsave = argv[1];
dataluncher:
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
			//argarray.push_back("fat:/GBADS/bios.bin");
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
			
		}
			//argarray.at(0) = filePath;
			//iprintf ("Running %s with %d parameters\n", argarray[0], argarray.size());
			//while(1);
			int err = runNdsFile (argarray[0], argarray.size(), (const char **)&argarray[0]);
			iprintf ("Start failed. Error %i\n", err);

		while(argarray.size() !=0 ) {
			free(argarray.at(0));
			argarray.erase(argarray.begin());
		}

		for(int i = 0;i < 60;i++)swiWaitForVBlank();
		while (1) {
			swiWaitForVBlank();
			scanKeys();
			if (!(keysHeld() & KEY_A)) break;
		}

	return 0;
}

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

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/
#include "file_browse.h"

char biosPath[MAXPATHLEN * 2];
char patchPath[MAXPATHLEN * 2];
char savePath[MAXPATHLEN * 2];
char szFile[MAXPATHLEN * 2];
char temppath[MAXPATHLEN * 2];


#include "ichflysettings.h"
#ifdef standalone
#include "Util.h"
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <nds.h>

//#include "fat.h"

extern "C" u32 fileRead (char* buffer, u32 cluster, u32 startOffset, u32 length);


#define SCREEN_COLS 32
#define ENTRIES_PER_SCREEN 22
#define ENTRIES_START_ROW 2
#define ENTRY_PAGE_LENGTH 10


using namespace std;

char showbuff[0x10];

char* filetypsforemu [4] = {"gbafile (start emu)","savefile","bios","patch"};


struct DirEntry {
	string name;
	bool isDirectory;
} ;



bool nameEndsWith (const string& name, const string& extension) {

	if (name.size() == 0 || name.size() < extension.size() || extension.size() == 0) {
		return false;
	}
	
	return strcasecmp (name.c_str() + name.size() - extension.size(), extension.c_str()) == 0;
}

bool dirEntryPredicate (const DirEntry& lhs, const DirEntry& rhs) {

	if (!lhs.isDirectory && rhs.isDirectory) {
		return false;
	}
	if (lhs.isDirectory && !rhs.isDirectory) {
		return true;
	}
	return strcasecmp(lhs.name.c_str(), rhs.name.c_str()) < 0;
}

void getDirectoryContents (vector<DirEntry>& dirContents, const string& extension) {
	struct stat st;

	dirContents.clear();
	DIR *pdir = opendir ("."); 
	if (pdir == NULL) {
		iprintf ("Unable to open the directory.\n");
	} else {

		while(true) {
			DirEntry dirEntry;

			struct dirent* pent = readdir(pdir);
			if(pent == NULL) break;
				
			stat(pent->d_name, &st);
			dirEntry.name = pent->d_name;
			dirEntry.isDirectory = (st.st_mode & S_IFDIR) ? true : false;

			if (dirEntry.name.compare(".") != 0 /*&& (dirEntry.isDirectory || nameEndsWith(dirEntry.name, extension))*/) {
				dirContents.push_back (dirEntry);
			}

		}
		
		closedir(pdir);
	}	
	
	sort(dirContents.begin(), dirContents.end(), dirEntryPredicate);
}

void getDirectoryContents (vector<DirEntry>& dirContents) {
	getDirectoryContents (dirContents, "");
}

void showDirectoryContents (const vector<DirEntry>& dirContents, int startRow) {
	char path[PATH_MAX];
	
	
	getcwd(path, PATH_MAX);
	
	// Clear the screen
	iprintf ("\x1b[2J");
	
	// Print the path
	if (strlen(path) < SCREEN_COLS) {
		iprintf ("%s", path);
	} else {
		iprintf ("%s", path + strlen(path) - SCREEN_COLS);
	}
	
	// Move to 2nd row
	iprintf ("\x1b[1;0H");
	// Print line of dashes
	iprintf ("--------------------------------");
	
	// Print directory listing
	for (int i = 0; i < ((int)dirContents.size() - startRow) && i < ENTRIES_PER_SCREEN; i++) {
		const DirEntry* entry = &dirContents.at(i + startRow);
		char entryName[SCREEN_COLS + 1];
		
		// Set row
		iprintf ("\x1b[%d;0H", i + ENTRIES_START_ROW);
		
		if (entry->isDirectory) {
			strncpy (entryName, entry->name.c_str(), SCREEN_COLS);
			entryName[SCREEN_COLS - 3] = '\0';
			iprintf (" [%s]", entryName);
		} else {
			strncpy (entryName, entry->name.c_str(), SCREEN_COLS);
			entryName[SCREEN_COLS - 1] = '\0';
			iprintf (" %s", entryName);
		}
	}
}

void printgbainfo (const char* filename)  {
	struct stat st;
	FILE *file = fopen(filename, "r");
	
	
	fread((char*)&gbaheader, 1, sizeof(gbaHeader_t),file);


	strncpy(showbuff,gbaheader.title,0xC);
	showbuff[0xC] = 0;
	iprintf("Name: %s\r\n",showbuff);
	iprintf("Version: %u\r\n",gbaheader.version);
	strncpy(showbuff,gbaheader.title,0x4);
	showbuff[0x4] = 0;
	iprintf("code: %s\r\n",gbaheader.gamecode);
	if(gbaheader.gamecode[0] > 0x40 && gbaheader.gamecode[0] < 0x46)iprintf("gen%u\r\n",(gbaheader.gamecode[0] - 0x40));
	else  if(gbaheader.gamecode[0] == 0x46) iprintf("Classic NES Series\r\n");
	else  if(gbaheader.gamecode[0] == 0x4B) iprintf("acceleration sensor\r\n");
	else  if(gbaheader.gamecode[0] == 0x50) iprintf("dot-code scanner\r\n");
	else  if(gbaheader.gamecode[0] == 0x52) iprintf("rumble and z-axis gyro sensor\r\n");
	else  if(gbaheader.gamecode[0] == 0x55) iprintf("RTC and solar sensor\r\n");
	else  if(gbaheader.gamecode[0] == 0x56) iprintf("rumble\r\n");
	else iprintf("?\r\n");
	iprintf("dest: ");
	if(gbaheader.gamecode[3] == 'J') iprintf("Japan\r\n");
	else  if(gbaheader.gamecode[3] == 'E') iprintf("USA/English\r\n");
	else  if(gbaheader.gamecode[3] == 'P') iprintf("Europe/Elsewhere\r\n");
	else  if(gbaheader.gamecode[3] == 'D') iprintf("German\r\n");
	else  if(gbaheader.gamecode[3] == 'F') iprintf("French\r\n");
	else  if(gbaheader.gamecode[3] == 'I') iprintf("Italian\r\n");
	else  if(gbaheader.gamecode[3] == 'S') iprintf("Spanish\r\n");
	else{ iprintf("?\r\n");}

	//showbuff[0x5] = 0;
	showbuff[0x2] = 0;
	strncpy(showbuff,gbaheader.makercode,0x2);

	iprintf("makercode: %s\r\n",showbuff);
	iprintf("Unitcode: %u\r\nDevicecode: %u\r\n",gbaheader.unitcode,gbaheader.devicecode);
	if(gbaheader.is96h == 0x96)iprintf("is96h OK\r\n");
	else {iprintf("is96h is %02x\r\n",gbaheader.is96h);}
	iprintf("Cartridge Key Number MSB %u\r\n",(gbaheader.logo[154] & 0x3));
	if(gbaheader.logo[152] == 0xA5)iprintf("Debugging Enabled\r\n");
	if(gbaheader.logo[152] == 0x21)iprintf("Debugging Disabled\r\n");
	if((gbaheader.entryPoint & 0xFE000000) == 0xEA000000){iprintf("entrypoint %08x\r\n",0x08000000 + (gbaheader.entryPoint & 0x00FFFFFF)*4 + 8);}
	else {iprintf("entrypoint not detected\r\n");}
	iprintf ("--------------------------------");
	fclose(file);
}



void browseForFile (const string& extension) {
	int pressed = 0;
	int screenOffset = 0;
	int fileOffset = 0;
	vector<DirEntry> dirContents;
	
	getDirectoryContents (dirContents, extension);

	showDirectoryContents (dirContents, screenOffset);
	
	while (true) 
	{
		// Clear old cursors
		for (int i = ENTRIES_START_ROW; i < ENTRIES_PER_SCREEN + ENTRIES_START_ROW; i++) {
			iprintf ("\x1b[%d;0H ", i);
		}
		// Show cursor
		iprintf ("\x1b[%d;0H*", fileOffset - screenOffset + ENTRIES_START_ROW);
		
		// Power saving loop. Only poll the keys once per frame and sleep the CPU if there is nothing else to do
		do {
			scanKeys();
			pressed = keysDownRepeat();
			//swiWaitForVBlank();
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			
		} while (!pressed);
	
		if (pressed & KEY_UP) 		fileOffset -= 1;
		if (pressed & KEY_DOWN) 	fileOffset += 1;
		if (pressed & KEY_LEFT) 	fileOffset -= ENTRY_PAGE_LENGTH;
		if (pressed & KEY_RIGHT)	fileOffset += ENTRY_PAGE_LENGTH;
		
		if (fileOffset < 0) 	fileOffset = dirContents.size() - 1;		// Wrap around to bottom of list
		if (fileOffset > ((int)dirContents.size() - 1))		fileOffset = 0;		// Wrap around to top of list

		// Scroll screen if needed
		if (fileOffset < screenOffset) 	{
			screenOffset = fileOffset;
			showDirectoryContents (dirContents, screenOffset);
		}
		if (fileOffset > screenOffset + ENTRIES_PER_SCREEN - 1) {
			screenOffset = fileOffset - ENTRIES_PER_SCREEN + 1;
			showDirectoryContents (dirContents, screenOffset);
		}
		
		if (pressed & KEY_A) {
			DirEntry* entry = &dirContents.at(fileOffset);
			if (entry->isDirectory) {
				// Enter selected directory
				chdir (entry->name.c_str());
				getDirectoryContents (dirContents, extension);
				screenOffset = 0;
				fileOffset = 0;
				showDirectoryContents (dirContents, screenOffset);
			}
			else 
			{
				// Clear the screen
				iprintf ("\x1b[2J");

					//ichfly some sort wtf


				int ausgewauhlt = 0;
				int pressed;
				bool nichtausgewauhlt = true;

				int pathLen = 0;

				getcwd (temppath, MAXPATHLEN);
				pathLen = strlen (temppath);
				strcpy (temppath + pathLen, entry->name.c_str());
				if(utilIsGBAImage(temppath)) ausgewauhlt = 0;
				if(utilIsSAV(temppath))ausgewauhlt = 1;

				while(nichtausgewauhlt)
				{
					iprintf("\x1b[2J");

					printgbainfo(temppath);
					
					iprintf("Use as\n");
					

					for(int i = 0; i < 4; i++)
					{
						if(i == ausgewauhlt) iprintf("->");
						else iprintf("  ");
						iprintf(filetypsforemu[i]);
						iprintf("\n");
					}
					while(nichtausgewauhlt)
					{
						// Power saving loop. Only poll the keys once per frame and sleep the CPU if there is nothing else to do
						do {
						//swiWaitForVBlank();
						if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
						while(!(REG_DISPSTAT & DISP_IN_VBLANK));
						scanKeys();
						pressed = keysDownRepeat();
						} while (!pressed);

						if (pressed&KEY_A)
						{
							switch(ausgewauhlt)
							{
								case 0:
							{
								getcwd (szFile, MAXPATHLEN);
								pathLen = strlen (szFile);
								strcpy (szFile + pathLen, entry->name.c_str());
								if(savePath[0] == 0)
								{
										sprintf(savePath,"%s.sav",szFile);
									    FILE *pFile = fopen(savePath, "r");
										if(pFile==NULL)savePath[0] = 0;
										fclose(pFile);
								}
								if(patchPath[0] == 0)
								{
										sprintf(patchPath,"%s.pat",szFile);
									    FILE *pFile = fopen(patchPath, "r");
										if(pFile==NULL)patchPath[0] = 0;
										fclose(pFile);
								}
								return;
							}

								case 1:
							{
								getcwd (savePath, MAXPATHLEN);
								pathLen = strlen (savePath);
								strcpy (savePath + pathLen, entry->name.c_str());
								nichtausgewauhlt = false;
								break;
							}
								case 2:
							{
								getcwd (biosPath, MAXPATHLEN);
								pathLen = strlen (biosPath);
								strcpy (biosPath + pathLen, entry->name.c_str());
								nichtausgewauhlt = false;
								break;
							}
								case 3:
							{
								getcwd (patchPath, MAXPATHLEN);
								pathLen = strlen (patchPath);
								strcpy (patchPath + pathLen, entry->name.c_str());
								nichtausgewauhlt = false;
								break;
							}
							}
						}
						if (pressed&KEY_DOWN && ausgewauhlt != 3){ ausgewauhlt++; break;}
						if (pressed&KEY_UP && ausgewauhlt != 0) {ausgewauhlt--; break;}
					}
				}
				showDirectoryContents (dirContents, screenOffset);
							//ichfly some sort wtf end
			}
		}
		
		if (pressed & KEY_B) {
			// Go up a directory
			chdir ("..");
			getDirectoryContents (dirContents, extension);
			screenOffset = 0;
			fileOffset = 0;
			showDirectoryContents (dirContents, screenOffset);
		}
	}
}

#endif
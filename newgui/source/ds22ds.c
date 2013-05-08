#include <nds.h>
#include <stdio.h>
#include <sys/stat.h>

#include <string.h>
#include <unistd.h>


#include "input.h"
#include "common.h"
#include "gui.h"
#include "main.h"
#include "memory.h"
#include "draw.h"

#include "gu.h"

#include "nds_loader_arm9.h"

void ds2_setSupend() //don't know what is dose 
{

}
int getSysTime() //todo ichfly
{
	return 0;
}
void ds2_setBacklight(int backlight) //todo ichfly
{

}
int ds2_setCPUclocklevel(unsigned int num)//ichfly haha
{
	return 1;
}

int argc;
char **argv;

bool Get_Args(char *file, char **filebuf)
{
	if(argc == 1 || argc == 2)strcpy( filebuf[0], argv[0] );
	if(argc == 2) strcpy( filebuf[1], argv[1] );
}

void* up_screen_addr;
void* down_screen_addr;
int bg;
int bgi;



void initich(int argca, char **argva)
{
	argc = argca;
	argv = argva;

	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	
	bgi = bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256,0,0);
	bg = bgInit(2, BgType_Bmp16, BgSize_B16_256x256,0,0);
	
	
	u16* buffer = (u16*)bgGetGfxPtr(bg);
	int iy = 0;
	
	while( iy < 256)
	{
		int ix = 0;
		while(ix < 256)
		{
			buffer[ix + iy * 256] = rand();
			ix++;
		}
		iy++;
	}
		
	buffer = (u16*)bgGetGfxPtr(bgi);
	
	iy = 0;
	while(iy < 256)
	{
		int ix = 0;
		while(ix < 256)
		{
			buffer[ix + iy * 256] = rand();
			ix++;
		}
		iy++;
	}
		
	//scroll(bg, 256, 256);
	up_screen_addr = (void*)malloc(2*256*192);
	down_screen_addr = (void*)malloc(2*256*192);
}
void ds2_flipScreen(enum SCREEN_ID screen_num, int done)
{
	if(screen_num & UP_SCREEN)
	{
		u16* buffer = (u16*)bgGetGfxPtr(bg);
		int iy = 0;
		while(iy < 256)
		{
			int ix = 0;
			while(ix < 256)
			{
				buffer[ix + iy * 256] = *(u16*)((u32)up_screen_addr + (ix* 2 + iy * 256* 2)) | 0x8000;
				ix++;
			}
			 iy++;
		}
	}
	if(screen_num & DOWN_SCREEN)
	{
		u16* buffer = (u16*)bgGetGfxPtr(bgi);
		int iy = 0;
		while(iy < 256)
		{
			int ix = 0;
			while(ix < 256)
			{
				buffer[ix + iy * 256] = *(u16*)((u32)down_screen_addr + (ix* 2 + iy * 256* 2)) | 0x8000;
				ix++;
			}
			iy++;
		}
	}
}
void ds2_clearScreen(enum SCREEN_ID screen_num, unsigned short color)
{
	if(screen_num & UP_SCREEN)
	{
		int iy = 0;
		while(iy < 192)
		{
			int ix = 0;
			while(ix < 256)
			{
				*(u16*)((u32)up_screen_addr + (ix* 2 + iy * 256* 2)) = color | 0x8000;
				ix++;
			}
			 iy++;
		}
	}
	if(screen_num & DOWN_SCREEN)
	{
		int iy = 0;
		while(iy < 192)
		{
			int ix = 0;
			while(ix < 256)
			{
				*(u16*)((u32)down_screen_addr + (ix* 2 + iy * 256* 2)) = color | 0x8000;
				ix++;
			}
			iy++;
		}
	}
}




//ichfly some things

void change_ext(char *src, char *buffer, char *extension)
{
  char *dot_position;
  strcpy(buffer, src);
  dot_position = strrchr(buffer, '.');

  if(dot_position)
    strcpy(dot_position, extension);
}

const u8 SVS_HEADER[SVS_HEADER_SIZE] = {'N', 'G', 'B', 'A', 'R', 'T', 'S', '1', '.', '0',
  'e'};

/*
 * Loads a saved state, given its file name and a file handle already opened
 * in at least mode "rb" to the same file. This function is responsible for
 * closing that file handle.
 * Assumes the gamepak used to save the state is the one that is currently
 * loaded.
 * Returns 0 on success, non-zero on failure.
 */
u32 load_state(char *savestate_filename, FILE *fp) //todo
{
	return 1;
}
void reset_gba() //todo
{
}
void update_backup_force() //todo
{
}
u32 reg[64]; //todo
char ichgamefileplpath[MAX_FILE];
s32 load_gamepak(char *file_path) //todo
{
	strcpy(ichgamefileplpath, file_path);
	return 0;
}

u32 save_state(char *savestate_filename, u16 *screen_capture)
{
	return 1;
}

void quit()
{

#ifdef USE_DEBUG
	fclose(g_dbg_file);
#endif
	while (1) {
		swiWaitForVBlank();
	}
}
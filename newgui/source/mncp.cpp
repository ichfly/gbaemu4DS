#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <sys/stat.h>

#include <string.h>
#include <unistd.h>


void maino(int argc, char **argv);

int main(int argc, char **argv) 
{
	// install the default exception handler
    defaultExceptionHandler();

	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

	if (!fatInitDefault()) {
		iprintf ("fatinitDefault failed!\n");
		while(1);
	} 

	//while(1);
	maino(argc,argv);
	return 0;
}
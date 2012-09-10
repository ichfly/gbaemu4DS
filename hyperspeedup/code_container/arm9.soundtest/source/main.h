#ifndef __MAIN_H__
#define __MAIN_H__

#include <nds.h>


#include <stdio.h>
#define Log(...) iprintf(__VA_ARGS__)
void VblankHandler();
void frameasyncsync();
void pausemenue();
//extern "C" PrintConsole* consoleDemoInitsubsc(void);

#endif /*__MAIN_H__*/

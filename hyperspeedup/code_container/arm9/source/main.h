#ifndef main_gba_core
#define main_gba_core

#include <nds.h>
#include <stdio.h>
#include <assert.h>
#include <nds/arm9/dldi.h>
#include <nds/disc_io.h>
#include <dirent.h>
#include "GBA.h"

#define INT_TABLE_SECTION __attribute__((section(".itcm")))
#define GBA_EWRAM ((void*)(0x02000000))

#define DEFAULT_CACHE_PAGES 16
#define DEFAULT_SECTORS_PAGE 8
   
#endif


#ifdef __cplusplus
extern "C" {
#endif

// The only built in driver
extern DLDI_INTERFACE _io_dldi_stub;

extern struct IntTable irqTable[MAX_INTERRUPTS] INT_TABLE_SECTION;
extern void __irqSet(u32 mask, IntFn handler, struct IntTable irqTable[] );


// The only built in driver
extern DLDI_INTERFACE _io_dldi_stub;

extern int frameskip;

extern int main( int argc, char **argv);

extern u8 arm7exchangefild[0x100];

extern gbaHeader_t gbaheader;
#ifdef __cplusplus
}
#endif

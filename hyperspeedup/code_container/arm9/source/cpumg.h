#ifndef __CPU_H__
#define __CPU_H__

#include <nds/jtypes.h>

// void cpu_GbaMemPerm();
// void cpu_NdsMemPerm();

// extern void cpu_GbaSetIwram();

extern "C" void cpu_ArmJump(u32 address, u32 r1);
extern "C" void cpu_ArmJumpforstackinit(u32 address, u32 r1);
void exInitswisystem(void (*customswiHdl)());
void gbaswieulatedbios();

void gbaMode2();

void gbaInit(bool slow);
void switch_to_unprivileged_mode();
void gbaMode();
void ndsMode();
u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState);
unsigned long ARMShift(unsigned long value,unsigned char shift);

extern "C" void readbankedextra(u32 CPSR);
extern "C" u32 cpuGetCPSR();
extern "C" u32 cpuGetSPSR();
extern "C" void cpuSetCPSR(u32 CPSR);

extern "C" u32 readbankedsp(u32 CPSR);
extern "C" u32 readbankedlr(u32 CPSR);

void cpupausemodeexit();
void cpupausemode();
void debugDump();


extern "C" void ichflyswiHalt();
extern "C" void ichflyswiWaitForVBlank();
extern "C" void ichflyswiIntrWait(u32 i,u32 c);




#endif /*__CPU_H__*/
 
 

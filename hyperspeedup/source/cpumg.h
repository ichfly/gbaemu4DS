#ifndef __CPU_H__
#define __CPU_H__

#include <nds/jtypes.h>

// void cpu_GbaMemPerm();
// void cpu_NdsMemPerm();

// extern void cpu_GbaSetIwram();

extern "C" void cpu_ArmJump(u32 address, u32 r1);

void exInitswisystem(void (*customswiHdl)());
void gbaswieulatedbios();

void gbaMode2();

void gbaInit();
void switch_to_unprivileged_mode();
void gbaMode();
void ndsMode();
u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState);
unsigned long ARMShift(unsigned long value,unsigned char shift);

extern "C" u32 cpuGetCPSR();
extern "C" void cpuSetCPSR(u32 CPSR);

#endif /*__CPU_H__*/
 
 

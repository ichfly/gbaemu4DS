#ifndef __CPU_H__
#define __CPU_H__

#include <nds/jtypes.h>

// void cpu_GbaMemPerm();
// void cpu_NdsMemPerm();

// extern void cpu_GbaSetIwram();

extern "C" void cpu_ArmJump(u32 address, u32 r1);

void gbaInit();
void gbaMode();
void ndsMode();

extern "C" u32 cpuGetCPSR();

#endif /*__CPU_H__*/
 
 

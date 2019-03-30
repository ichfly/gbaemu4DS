#include <nds/ndstypes.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void __attribute__((section(".dtcm"))) (*exHandler)();
extern void __attribute__((section(".dtcm"))) (*exHandlerswi)();
extern void __attribute__((section(".dtcm"))) (*exHandlerundifined)();

extern u32  __attribute__((section(".dtcm"))) exRegs[];
extern u32  __attribute__((section(".dtcm"))) BIOSDBG_SPSR;

extern void readbankedextra(u32 CPSR);
extern u32 cpuGetCPSR();
extern u32 cpuGetSPSR();
extern void cpuSetCPSR(u32 CPSR);

extern u32 readbankedsp(u32 CPSR);
extern u32 readbankedlr(u32 CPSR);

extern void cpupausemodeexit();
extern void cpupausemode();

extern char disbuffer[0x2000];

extern void debugDump();
extern void failcpphandler();

extern void exInitundifinedsystem(void (*)());
extern void exInitswisystem(void (*)());
extern void exInit(void (*)());

extern void undifinedresolver();

extern void gbaInit(bool useMPUFast);
extern void switch_to_unprivileged_mode();
extern void gbaMode();
extern void gbaMode2();

extern void ndsMode();
extern u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState);
extern unsigned long ARMShift(unsigned long value,unsigned char shift);

extern void BIOScall(int op,  u32 *R);

extern void cpu_ArmJump(u32 address, u32 r1);
extern void cpu_ArmJumpforstackinit(u32 address, u32 r1);
extern void exInitswisystem(void (*customswiHdl)());
extern void gbaswieulatedbios();

//cpu_s.s
extern void ichflyswiHalt();
extern void ichflyswiWaitForVBlank();
extern void ichflyswiIntrWait(u32 i,u32 c);

extern void switch_to_unprivileged_mode();
extern void emulateedbiosstart();
extern void downgreadcpu();

extern void puGba();
extern void puNds();

extern void ARMV5toARMV4Mode();

//GBA SWI sleep mode (swi 0x3)
extern void backup_mpu_setprot();
extern void restore_mpu_setprot();
extern u32 MPUPERMBACKUPSET_SWI;	//MPUd/itcmmemorypermissionsfromcaller
extern void IRQWait(uint32 reentrant,uint32 irqstowait);

#ifdef __cplusplus
}
#endif
 

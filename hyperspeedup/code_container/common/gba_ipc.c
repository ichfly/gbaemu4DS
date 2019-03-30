#include <nds.h>
#include <time.h>
#include "gba_ipc.h"

#ifdef ARM9
#include "../cpumg.h"
#endif

//GBA RTC
//gbaemu4ds clock opcodes
u8 gba_get_yearbytertc(){
	return (u8)GBA_IPC->clockdata[0];
}

u8 gba_get_monthrtc(){
	return (u8)GBA_IPC->clockdata[1];
}

u8 gba_get_dayrtc(){
	return (u8)GBA_IPC->clockdata[2];
}

u8 gba_get_dayofweekrtc(){
	return (u8)GBA_IPC->clockdata[3];
}


u8 gba_get_hourrtc(){
	return (u8)GBA_IPC->clockdata[4];
}

u8 gba_get_minrtc(){
	return (u8)GBA_IPC->clockdata[5];
}

u8 gba_get_secrtc(){
	return (u8)GBA_IPC->clockdata[6];
}


#ifdef ARM7
void SendArm9Command(u32 command1, u32 command2){
#endif
#ifdef ARM9
__attribute__((section(".itcm")))
void SendArm7Command(u32 command1, u32 command2){
#endif
    REG_IPC_FIFO_TX = (u32)command1;
    REG_IPC_FIFO_TX = (u32)command2;
}


//new GBA SWI sleepmode
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void setARM7asleep(bool value){
	GBA_IPC->arm7asleep = value;
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
bool getARM7asleep(){
	return (bool)GBA_IPC->arm7asleep;
}

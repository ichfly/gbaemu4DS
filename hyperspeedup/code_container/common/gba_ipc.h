//shared static object across both ARM7 and ARM9 cpu (copied to shared region memory)

#ifndef SHARED_IPC_defs
#define SHARED_IPC_defs

#include <nds.h>

//gbaemu4ds
#define neu_sound_16fifo

//---------------------------------------------------------------------------------
struct sIPCSharedGBA{
//---------------------------------------------------------------------------------
    int16 touchX,   touchY;   // raw x/y
	int16 touchXpx, touchYpx; // TFT x/y pixel

	int16 touchZ1,  touchZ2;  // TSC x-panel measurements
	uint16 tdiode1,  tdiode2;  // TSC temperature diodes
	uint32 temperature;        // TSC computed temperature

	uint16 buttons;            // keypad buttons
    uint16 buttons_xy_folding;  // X, Y, /PENIRQ buttons

    //unused
	union {
		uint8 curtime[8];        // current time response from RTC

        struct {
                u8 rtc_command;
                u8 rtc_year;           //add 2000 to get 4 digit year
                u8 rtc_month;          //1 to 12
                u8 rtc_day;            //1 to (days in month)
        
                u8 rtc_incr;
                u8 rtc_hours;          //0 to 11 for AM, 52 to 63 for PM
                u8 rtc_minutes;        //0 to 59
                u8 rtc_seconds;        //0 to 59
        };
    };
    
	u8 touched;				    //TSC touched?
    u8 touch_pendown;           //TSC already held before?
    
	uint16 battery;             // battery life status
	uint16 aux;                 // SPI AUX port status

    vuint8 mailBusy;              //checks if IPC between ARM Cores is in use (1), 0 otherwise.
  
    u32 rom_entrypoint;
    
    //IPC Clock
    //[0]; //yy
    //[1]; //mth
    //[2]; //dd
    //[3]; //day of week
    //[4]; //hh
    //[5]; //mm
    //[6]; //ss
    u8 clockdata[0x20];
    
	bool arm7asleep;
	
	//not yet!
	/*
    //GBA IRQS
    u16 GBAIE;

    u16 GBAIF;

    u16 GBAIME;
    
    //The next hardware mapped here: DMA 1,2 and Timers 0,1 since they belong to GBA sound system
    //FIFO DMA Emulation
    
    //40000A0h - FIFO_A_L - Sound A FIFO, Data 0 and Data 1 (W)
    u8 FIFO_A_L[2];
    //40000A2h - FIFO_A_H - Sound A FIFO, Data 2 and Data 3 (W)
    u8 FIFO_A_H[2];
    
    //These two registers may receive 32bit (4 bytes) of audio data (Data 0-3, Data 0 being located in least significant byte which is replayed first).
    //Internally, the capacity of the FIFO is 8 x 32bit (32 bytes), allowing to buffer a small amount of samples. As the name says (First In First Out), oldest data is replayed first.

    //40000A4h - FIFO_B_L - Sound B FIFO, Data 0 and Data 1 (W)
    u8 FIFO_B_L[2];
    //40000A6h - FIFO_B_H - Sound B FIFO, Data 2 and Data 3 (W)
    u8 FIFO_B_H[2];
    u8 fifo_processed;  //0 idle / 1 ARM7/ARM9 process IO FIFO DMA ports
    
    
    //DMA
    u32 dma0Source;
    u32 dma0Dest;
    u16 GBADM0SAD_L;
    u16 GBADM0SAD_H;
    u16 GBADM0DAD_L;
    u16 GBADM0DAD_H;
    u16 GBADM0CNT_L;
    u16 GBADM0CNT_H;
    
    u32 dma1Source;
    u32 dma1Dest;
    u16 GBADM1SAD_L;
    u16 GBADM1SAD_H;
    u16 GBADM1DAD_L;
    u16 GBADM1DAD_H;
    u16 GBADM1CNT_L;
    u16 GBADM1CNT_H;
    
    u32 dma2Source;
    u32 dma2Dest;
    u16 GBADM2SAD_L;
    u16 GBADM2SAD_H;
    u16 GBADM2DAD_L;
    u16 GBADM2DAD_H;
    u16 GBADM2CNT_L;
    u16 GBADM2CNT_H;
    
    u32 dma3Source;
    u32 dma3Dest;
    u16 GBADM3SAD_L;
    u16 GBADM3SAD_H;
    u16 GBADM3DAD_L;
    u16 GBADM3DAD_H;
    u16 GBADM3CNT_L;
    u16 GBADM3CNT_H;
    
    //Timers
    u16 timer0Value;
    bool timer0On;
    int timer0Ticks;
    int timer0Reload;
    int timer0ClockReload;
    
    u16 GBATM0D;
    u16 GBATM0CNT;
    
    u16 timer1Value;
    bool timer1On;
    int timer1Ticks;
    int timer1Reload;
    int timer1ClockReload;
    
    u16 GBATM1D;
    u16 GBATM1CNT;
    
    u16 timer2Value;
    bool timer2On;
    int timer2Ticks;
    int timer2Reload;
    int timer2ClockReload;
    
    u16 GBATM2D;
    u16 GBATM2CNT;
    
    u16 timer3Value;
    bool timer3On;
    int timer3Ticks;
    int timer3Reload;
    int timer3ClockReload;
    
    u16 GBATM3D;
    u16 GBATM3CNT;

    //GBA Virtual Sound Controller
    u16 SOUNDCNT_L;
    u16 SOUNDCNT_H;
        
    */

};

//4000136h - NDS7 - EXTKEYIN - Key X/Y Input (R)
//  0      Button X     (0=Pressed, 1=Released)
//  1      Button Y     (0=Pressed, 1=Released)
//  3      DEBUG button (0=Pressed, 1=Released/None such)
//  6      Pen down     (0=Pressed, 1=Released/Disabled) (always 0 in DSi mode)
//  7      Hinge/folded (0=Open, 1=Closed)
//  2,4,5  Unknown / set
//  8..15  Unknown / zero

#define KEY_XARM7 (1<<0)
#define KEY_YARM7 (1<<1)
#define KEY_HINGE (1<<7)

#define FIFO_SWI_SLEEPMODE (u32)(0xff1100aa 	& ~0xC0000000)
#define ARM9_REQ_SWI_TO_ARM7 (u32)(0xff1100ab	& ~0xC0000000)
#define ARM7_REQ_SWI_TO_ARM9 (u32)(0xff1100ac	& ~0xC0000000)

#define ARM7_TURNOFF_SOUND (u32)(0xff1100ad	& ~0xC0000000)
//ARM7_TURNON_SOUND is issued on LID open IRQ

#define GBA_IPC ((volatile struct sIPCSharedGBA*)0x027FF000)

#endif



#ifdef __cplusplus
extern "C"{
#endif

//gbaemu4ds clock opcodes
extern u8 gba_get_yearbytertc();
extern u8 gba_get_monthrtc();
extern u8 gba_get_dayrtc();
extern u8 gba_get_dayofweekrtc();
extern u8 gba_get_hourrtc();
extern u8 gba_get_minrtc();
extern u8 gba_get_secrtc();

#ifdef ARM7
extern void SendArm9Command(u32 command1, u32 command2);
#endif
#ifdef ARM9
extern void SendArm7Command(u32 command1, u32 command2);
#endif

extern bool ARM7asleep;
extern void setARM7asleep(bool value);
extern bool getARM7asleep();

#ifdef __cplusplus
}
#endif

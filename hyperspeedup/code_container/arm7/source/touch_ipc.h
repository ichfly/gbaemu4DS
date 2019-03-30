#ifndef GBAEMU4DS_ARM7TOUCH
#define GBAEMU4DS_ARM7TOUCH

#define TSC_MEASURE_TEMP1    0x84
//#define TSC_MEASURE_Y        0x91
#define TSC_MEASURE_BATTERY  0xA4
//#define TSC_MEASURE_Z1       0xB1
//#define TSC_MEASURE_Z2       0xC1
//#define TSC_MEASURE_X        0xD1
//#define TSC_MEASURE_AUX      0xE4
#define TSC_MEASURE_TEMP2    0xF4

#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void gbaemu4ds_ipc();

#ifdef __cplusplus
}
#endif

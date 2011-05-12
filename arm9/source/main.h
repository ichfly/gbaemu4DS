#ifndef __MAIN_H__
#define __MAIN_H__

#include <nds.h>

#define REG_DISPCNT DISPLAY_CR
#define REG_BG0CNT BG0_CR

#include <stdio.h>
#define Log(...) iprintf(__VA_ARGS__)

#endif /*__MAIN_H__*/

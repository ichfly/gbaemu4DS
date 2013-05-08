#include <nds.h>
#include <nds/arm7/audio.h>

#include "../../gloabal/cpuglobal.h"

#include "timer20.h"

#define minval 0x1000 //can cause delays

u32 soundLentimefild[8];
Functiondec lenfunctions[8];
u32 nexttimelen = 0xFFFF;
u32 nexttimelendurchlauf = 0xFFFF;

#define timeron
#ifdef timeron
void initimer()
{
	int i = 0;
	while(i < 8)
	{
		soundLentimefild[i] = 0;
		//soundothertimefild[i] = 0;
		i++;
	}
	timerStart(2, ClockDivider_1,0x10000-0xFFFF, timerlen); //512 Hz
}
void timerlen()
{
	u32 min = 0xFFFF;
	int i = 0;
	while(i < 8)
	{
		if(soundLentimefild[i] != 0)
		{
			if(1 == soundLentimefild[i])
			{
				soundLentimefild[i] = 0xFFFFFFFF;
			}
			else if(0xFFFFFFFF == soundLentimefild[i])
			{
				//jump
				u32 temp5 = lenfunctions[i](i);
				if(temp5 != 0)soundLentimefild[i] = temp5 + nexttimelendurchlauf;
				else soundLentimefild[i] = 0;
			}
			else
			{
				soundLentimefild[i]-= nexttimelen;
				if(min > soundLentimefild[i])
				{
					min = (soundLentimefild[i] - 1);
				}
			}
			nexttimelendurchlauf = nexttimelen;
			nexttimelen = min;
			min = (min > minval) ? min :  minval;
			TIMER_DATA(2) = 0x10000-min;
		}
		i++;
	}
}
void timerlenadd(u8 chan,u32 val,Functiondec func) //val min 0x20000
{
	if(val != 0)
	{
		val =- 0x10000-TIMER_DATA(2);
		val =+ nexttimelendurchlauf;
		soundLentimefild[chan] =  val;
		lenfunctions[chan] = func;
	}
	else
	{
		soundLentimefild[chan] =  0;
	}
}
#else
void initimer()
{
}
void timerlen()
{
}
void timerlenadd(u8 chan,u32 val,Functiondec func) //val min 0x20000
{
}
#endif
#include <nds.h>
#include <nds/arm7/audio.h>

#include "../../gloabal/cpuglobal.h"

#include "timer20.h"

u32 soundLentimefild[4];
Functiondec lenfunctions[4];
u32 nexttimelen = 0xFFFF;
u32 nexttimelendurchlauf = 0xFFFF;

u32 soundothertimefild[4];
Functiondec otherfunctions[4];
u32 nexttimeother = 0xFFFF;
u32 nexttimeotherdurchlauf = 0xFFFF;

void initimer()
{
	int i = 0;
	while(i < 4)
	{
		soundLentimefild[i] = 0;
		soundothertimefild[i] = 0;
		i++;
	}
	timerStart(2, ClockDivider_1,0x10000-0xFFFF, timerlen); //512 Hz
	timerStart(3, ClockDivider_1,0x10000-0xFFFF, timerother); //512 Hz
}
void timerlen()
{
	u32 min = 0xFFFF;
	int i = 0;
	while(i < 4)
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
void timerother()
{
	u32 min = 0xFFFF;
	int i = 0;
	while(i < 4)
	{
		if(soundothertimefild[i] != 0)
		{
			if(1 == soundothertimefild[i])
			{
				soundothertimefild[i] = 0xFFFFFFFF;
			}
			else if(0xFFFFFFFF == soundothertimefild[i])
			{
				//jump
				u32 temp5 = otherfunctions[i](i);
				if(temp5 != 0)soundothertimefild[i] = temp5 + nexttimeotherdurchlauf;
				else soundothertimefild[i] = 0;
			}
			else
			{
				soundothertimefild[i]-= nexttimeother;
				if(min > soundothertimefild[i])
				{
					min = (soundothertimefild[i] - 1);
				}
			}
			nexttimeotherdurchlauf = nexttimelen;
			nexttimeother = min;
			TIMER_DATA(3) = 0x10000-min;
		}
		i++;
	}
}
void timerotheradd(u8 chan,u32 val,Functiondec func) //val min 0x20000
{
	if(val != 0)
	{
		val =- 0x10000-TIMER_DATA(3);
		val =+ nexttimeotherdurchlauf;
		soundothertimefild[chan] =  val;
		otherfunctions[chan] = func;
	}
	else
	{
		soundothertimefild[chan] =  0;
	}
}
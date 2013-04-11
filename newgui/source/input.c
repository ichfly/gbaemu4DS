/* unofficial gameplaySP kai
 *
 * Copyright (C) 2006 Exophase <exophase@gmail.com>
 * Copyright (C) 2007 takka <takka@tfact.net>
 * Copyright (C) 2007 ????? <?????>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <sys/stat.h>

#include <string.h>
#include <unistd.h>

#include <dirent.h>

#include "input.h"
#include "gui.h"

#include "common.h"

TOUCH_SCREEN touch;

enum PspCtrlButtons
{
	/** Select button. */
	PSP_CTRL_SELECT     = 0x000001,
	/** Start button. */
	PSP_CTRL_START      = 0x000008,
	/** Up D-Pad button. */
	PSP_CTRL_UP         = 0x000010,
	/** Right D-Pad button. */
	PSP_CTRL_RIGHT      = 0x000020,
	/** Down D-Pad button. */
	PSP_CTRL_DOWN      	= 0x000040,
	/** Left D-Pad button. */
	PSP_CTRL_LEFT      	= 0x000080,
	/** Left trigger. */
	PSP_CTRL_LTRIGGER   = 0x000100,
	/** Right trigger. */
	PSP_CTRL_RTRIGGER   = 0x000200,
	/** Triangle button. */
	PSP_CTRL_TRIANGLE   = 0x001000,
	/** Circle button. */
	PSP_CTRL_CIRCLE     = 0x002000,
	/** Cross button. */
	PSP_CTRL_CROSS      = 0x004000,
	/** Square button. */
	PSP_CTRL_SQUARE     = 0x008000,
	/** Home button. */
	PSP_CTRL_HOME       = 0x010000,
	/** Hold button. */
	PSP_CTRL_HOLD       = 0x020000,
	/** Music Note button. */
	PSP_CTRL_NOTE       = 0x800000,
};

#ifndef NDS_LAYER
u32 readkey()
{
    unsigned char key;
	u32 ret;
	
	if(!serial_getc_noblock(&key))
	    return 0;
	
	switch(key){
		case 'i': ret= PSP_CTRL_UP;
					break;
		case 'k': ret= PSP_CTRL_DOWN;
					break;
		case 'j': ret= PSP_CTRL_LEFT;
					break;
		case 'l': ret= PSP_CTRL_RIGHT;
					break;
		case 'q': ret= PSP_CTRL_CIRCLE;
					break;
		case 'w': ret= PSP_CTRL_CROSS;
					break;
		case 'a': ret= PSP_CTRL_TRIANGLE;
					break;
		case 's': ret= PSP_CTRL_SQUARE;
					break;
		case 'z': ret= PSP_CTRL_SELECT;
					break;
		case 'x': ret= PSP_CTRL_START;
					break;
		case 'e': ret= PSP_CTRL_LTRIGGER;
					break;
		case 'r': ret= PSP_CTRL_RTRIGGER;
					break;
		default: ret= 0;
					break;
	}
	
	return ret;
}
#else
u32 readkey()
{
    touchPosition toucha;

	// read the button states
	scanKeys();

	// read the touchscreen coordinates
	touchRead(&toucha);

    touch.x = toucha.px;
    touch.y = toucha.py;
	return keysDown();
}
#endif

/*--------------------------------------------------------
	Wait any key in [key_list] pressed
	if key_list == NULL, return at any key pressed
--------------------------------------------------------*/
unsigned int wait_Anykey_press(unsigned int key_list)
{
	unsigned int key;

	while(1)
	{
		// read the button states
		scanKeys();
		key = keysDown();
		if(key) {
			if(0 == key_list) break;
			else if(key & key_list) break;
		}
		swiWaitForVBlank();
	}

	return key;
}

/*--------------------------------------------------------
	Wait all key in [key_list] released
	if key_list == NULL, return at all key released
--------------------------------------------------------*/
void wait_Allkey_release(unsigned int key_list)
{
	unsigned int key;
    struct key_buf inputdata;

	while(1)
	{
		// read the button states
		scanKeys();
		key = keysDown();

		if(0 == key) break;
		else if(!key_list) continue;
		else if(0 == (key_list & key)) break;
		swiWaitForVBlank();
	}
}



u32 key = 0;

u32 tilt_sensor_x;
u32 tilt_sensor_y;

#ifndef NDS_LAYER
TOUCH_SCREEN last_touch= {0, 0};

#define PSP_ALL_BUTTON_MASK 0x1FFFF
u32 button_repeat = 0;
u32 sensorR;

static u32 button_circle;
static u32 button_cross;

#endif

u32 last_buttons = 0;
//u64 button_repeat_timestamp;
u32 button_repeat_timestamp;

typedef enum
{
  BUTTON_NOT_HELD,
  BUTTON_HELD_INITIAL,
  BUTTON_HELD_REPEAT
} button_repeat_state_type;

button_repeat_state_type button_repeat_state = BUTTON_NOT_HELD;
#ifndef NDS_LAYER
gui_action_type cursor_repeat = CURSOR_NONE;
#else
unsigned int gui_button_repeat = 0;
#endif

//#define BUTTON_REPEAT_START    200000
//#define BUTTON_REPEAT_CONTINUE 50000
#ifndef NDS_LAYER
#define BUTTON_REPEAT_START    (200)    // Not sure what this 200 is
				// Just avoiding a compiler error [Neb]
#define BUTTON_REPEAT_CONTINUE (0)                        //之后重复间隔 0
#else
#define BUTTON_REPEAT_START (21428 / 2)
#define BUTTON_REPEAT_CONTINUE (21428 / 20)
#endif

// GUI输入处理

gui_action_type key_to_cursor(unsigned int key)
{
	switch (key)
	{
		case KEY_UP:	return CURSOR_UP;
		case KEY_DOWN:	return CURSOR_DOWN;
		case KEY_LEFT:	return CURSOR_LEFT;
		case KEY_RIGHT:	return CURSOR_RIGHT;
		case KEY_L:	return CURSOR_LTRIGGER;
		case KEY_R:	return CURSOR_RTRIGGER;
		case KEY_A:	return CURSOR_SELECT;
		case KEY_B:	return CURSOR_BACK;
		case KEY_X:	return CURSOR_EXIT;
		case KEY_TOUCH:	return CURSOR_TOUCH;
		default:	return CURSOR_NONE;
	}
}

static unsigned int gui_keys[] = {
	KEY_A, KEY_B, KEY_X, KEY_L, KEY_R, KEY_TOUCH, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

gui_action_type get_gui_input(void)
{
	gui_action_type	ret;

	//struct key_buf inputdata; //ichfly all this

	u32 ikey;
	scanKeys();
	ikey = keysDown();
	//ds2_getrawInput(&inputdata);

	/*if (inputdata.key & KEY_LID)
	{
		//ds2_setSupend(); //ichfly
		do {
			ds2_getrawInput(&inputdata);
			swiWaitForVBlank();//mdelay(1);
		} while (inputdata.key & KEY_LID);
		//ds2_wakeup();
		// In the menu, the lower screen's backlight needs to be on,
		// and it is on right away after resuming from suspend.
		// mdelay(100); // needed to avoid ds2_setBacklight crashing
		// ds2_setBacklight(3);
	}*/ //ichfly haha

	unsigned int i;
	while (1)
	{
		switch (button_repeat_state)
		{
		case BUTTON_NOT_HELD:
			// Pick the first pressed button out of the gui_keys array.
			for (i = 0; i < sizeof(gui_keys) / sizeof(gui_keys[0]); i++)
			{
				if (ikey & gui_keys[i])
				{
					button_repeat_state = BUTTON_HELD_INITIAL;
					button_repeat_timestamp = getSysTime();
					gui_button_repeat = gui_keys[i];
					return key_to_cursor(gui_keys[i]);
				}
			}
			return CURSOR_NONE;
		case BUTTON_HELD_INITIAL:
		case BUTTON_HELD_REPEAT:
			// If the key that was being held isn't anymore...
			if (!(ikey & gui_button_repeat))
			{
				button_repeat_state = BUTTON_NOT_HELD;
				// Go see if another key is held (try #2)
				break;
			}
			else
			{
				unsigned int IsRepeatReady = getSysTime() - button_repeat_timestamp >= (button_repeat_state == BUTTON_HELD_INITIAL ? BUTTON_REPEAT_START : BUTTON_REPEAT_CONTINUE);
				if (!IsRepeatReady)
				{
					// Temporarily turn off the key.
					// It's not its turn to be repeated.
					ikey &= ~gui_button_repeat;
				}
				
				// Pick the first pressed button out of the gui_keys array.
				for (i = 0; i < sizeof(gui_keys) / sizeof(gui_keys[0]); i++)
				{
					if (ikey & gui_keys[i])
					{
						// If it's the held key,
						// it's now repeating quickly.
						button_repeat_state = gui_keys[i] == gui_button_repeat ? BUTTON_HELD_REPEAT : BUTTON_HELD_INITIAL;
						button_repeat_timestamp = getSysTime();
						gui_button_repeat = gui_keys[i];
						return key_to_cursor(gui_keys[i]);
					}
				}
				// If it was time for the repeat but it
				// didn't occur, stop repeating.
				if (IsRepeatReady) button_repeat_state = BUTTON_NOT_HELD;
				return CURSOR_NONE;
			}
		}
	}
}

u32 button_input_to_gba[] =
{
    BUTTON_A,
    BUTTON_B,
    BUTTON_SELECT,
    BUTTON_START,
    BUTTON_RIGHT,
    BUTTON_LEFT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_R,
    BUTTON_L
};

u32 rapidfire_flag = 1;

u32 fast_backward= 0;
// 仿真过程输入


#if 0
void init_input()
{
  int button_swap;
  sceCtrlSetSamplingCycle(0);
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
  initHomeButton(sceKernelDevkitVersion());
  tilt_sensor_x = 0x800;
  tilt_sensor_y = 0x800;
  sceUtilityGetSystemParamInt(9, &button_swap);
  if (button_swap == 0)
  {
    button_circle = CURSOR_SELECT;
    button_cross  = CURSOR_EXIT;
  }
  else
  {
    button_circle = CURSOR_EXIT;
    button_cross  = CURSOR_SELECT;
  }
}
#endif

void init_input()
{
#ifndef NDS_LAYER
    button_circle = CURSOR_SELECT;
    button_cross  = CURSOR_EXIT;
#endif
}

// type = READ / WRITE_MEM
#define input_savestate_body(type)                                            \
{                                                                             \
  FILE_##type##_VARIABLE(g_state_buffer_ptr, key);                            \
}                                                                             \

/*void input_read_mem_savestate()
input_savestate_body(READ_MEM);

void input_write_mem_savestate()
input_savestate_body(WRITE_MEM);
*/
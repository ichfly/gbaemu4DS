#include <nds.h>
#include <nds/arm7/audio.h>

u16 callline = 0xFFFF;

//#define checkforerror


	u8* soundbuffA = 0;
	u8* soundbuffB = 0;

u8 dmaApart = 0;
u8 dmaBpart = 0;

u32 dmabuffer = 0;

u16 SOUNDCNT_L = 0;
u16 SOUNDCNT_H = 0;

u8 tacktgeber_sound_FIFO_DMA_A = 0;
u8 tacktgeber_sound_FIFO_DMA_B = 0;
u16 TM0CNT_L = 0;
u16 TM1CNT_L = 0;

u16 TM0CNT_H = 0;
u16 TM1CNT_H = 0;

u16 DMA1CNT_H = 0;
u16 DMA2CNT_H = 0;


u16 DMA1SAD_L = 0;
u16 DMA1SAD_H  = 0;
u16 DMA1DAD_L = 0;
u16 DMA1DAD_H  = 0;

u16 DMA2SAD_L = 0;
u16 DMA2SAD_H  = 0;
u16 DMA2DAD_L = 0;
u16 DMA2DAD_H  = 0;


//debug stuff

vu32 debugsrc1 = 0;
vu32 debugsrc2  = 0;
vu32 debugfr1 = 0;
vu32 debugfr2  = 0;

void senddebug32(u32 val)
{
	REG_IPC_FIFO_TX = 0x4000BEEF;
	REG_IPC_FIFO_TX = val;
}

void dmaAtimerinter()
{
	int oldIME = enterCriticalSection();
	REG_IPC_FIFO_TX = debugsrc1;
	debugsrc1+=16;
	while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
	int temp1 = REG_IPC_FIFO_RX;
	while(temp1 != 0x1) //can't happen else
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		newvalwrite(temp1 & ~0xC0000000,REG_IPC_FIFO_RX);
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		temp1 = REG_IPC_FIFO_RX;
	}
	int i = 0;
	while(i < 4)
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		*(u32*)(soundbuffA + i*4 + 0x10 * dmaApart) = REG_IPC_FIFO_RX;
		i++;
	}
	if(dmaApart == 0) dmaApart = 1;
	else dmaApart = 0;
	leaveCriticalSection(oldIME);
}
void dmaBtimerinter()
{
	int oldIME = enterCriticalSection();
	REG_IPC_FIFO_TX = debugsrc2;
	debugsrc2+=0x10;
	while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
	int temp1 = REG_IPC_FIFO_RX;
	while(temp1 != 0x1)  //my the irq is not tiggerd yet
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		newvalwrite(temp1& ~0xC0000000,REG_IPC_FIFO_RX);
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		temp1 = REG_IPC_FIFO_RX;
	}
	int i = 0;
	while(i < 4)
	{
		while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
		*(u32*)(soundbuffB + i*4 + 0x10 * dmaBpart) = REG_IPC_FIFO_RX;
		i++;
	}
	if(dmaBpart == 0) dmaBpart = 1;
	else dmaBpart = 0;
	leaveCriticalSection(oldIME);
}

bool autodetectdetect = false;


void newvalwrite(u32 addr,u32 val)
{
			int oldIME = enterCriticalSection();
			switch(addr)
			{
			  case 0xBC:
				DMA1SAD_L = val;
				break;
			  case 0xBE:
				DMA1SAD_H = val & 0x0FFF;
				break;
			  case 0xC0:
				DMA1DAD_L = val;
				checkstart();
				break;
			  case 0xC2:
				DMA1DAD_H = val & 0x07FF;
				checkstart();
				break;
			  case 0xC4:
				//DMA1CNT_L = val & 0x3FFF;
				break;
			  case 0xC6:
				DMA1CNT_H = val;
				checkstart();
				break;
			  case 0xC8:
				DMA2SAD_L = val;
				break;
			  case 0xCA:
				DMA2SAD_H = val & 0x0FFF;
				break;
			  case 0xCC:
				DMA2DAD_L = val;
				checkstart();
				break;
			  case 0xCE:
				DMA2DAD_H = val & 0x07FF;
				checkstart();
			  case 0xD0:
				//DMA2CNT_L = val & 0x3FFF;
				break;
			  case 0xD2:
				DMA2CNT_H = val;
				checkstart();
				break;





			case 0x100:
				TM0CNT_L = val;
				updatetakt();
			case 0x102:
				TM0CNT_H = val;
				updatetakt();
				checkstart();
			case 0x104:
				TM1CNT_L = val;
				updatetakt();
			case 0x106:
				TM1CNT_H = val;
				updatetakt();
				checkstart();
			case 0x80:
				SOUNDCNT_L = val;
				updatevol();
				break;
			case 0x82:
				SOUNDCNT_H = val; //Reset FIFO is not needed because we don't have direct streaming yet so don't need that
				updatevol();

				if(val & BIT(10))
				{
					tacktgeber_sound_FIFO_DMA_A = 1;
				}
				else
				{
					tacktgeber_sound_FIFO_DMA_A = 0;
				}
				if(val & BIT(14))
				{
					tacktgeber_sound_FIFO_DMA_B = 1;
				}
				else
				{
					tacktgeber_sound_FIFO_DMA_B = 0;
				}
				updatetakt();
				checkstart();
				break;
			case 0x84:
				if(val & 0x80)REG_SOUNDCNT |= 0x8000;
				else
				{
					REG_SOUNDCNT &= ~0x8000;
				}
				break;
			case 0x88:
					  //Amplitude Resolution/Sampling Cycle is not supported so only Bias
					  //it is better on the DS any way
				  REG_SOUNDBIAS = val;
				  break;


			  case 0x1FFFFFFA:
				  dmabuffer = val;
				  soundbuffA = (u32*)(dmabuffer);
					SCHANNEL_SOURCE(4) = soundbuffA;
				  soundbuffB = (u32*)(dmabuffer + 0x50);
					SCHANNEL_SOURCE(5) = soundbuffB;
					break;
			case 0x1FFFFFFB: //wait
				if(autodetectdetect/* && (REG_VCOUNT > 160 || REG_VCOUNT < callline)*/)
				{
					REG_IPC_FIFO_TX = 0x4100BEEF; //send cmd 0x4100BEEF
				}
				break;
			case 0x1FFFFFFC: //setauto
				autodetectdetect = true;
				break;
			case 0x1FFFFFFFD: //getkeys
				{
					touchPosition tempPos = {0};
					u16 keys= REG_KEYXY;
					if(!touchPenDown()) {
						keys |= KEY_TOUCH;
  					} else {
						keys &= ~KEY_TOUCH;
					}
					touchReadXY(&tempPos);	
					REG_IPC_FIFO_TX = 1; //send cmd 1
					REG_IPC_FIFO_TX = keys;
					REG_IPC_FIFO_TX = tempPos.px;
					REG_IPC_FIFO_TX = tempPos.py;
				}
				break;
			case 0x1FFFFFFF: //set callline
				callline = val;
				break;
			default:
				//senddebug32(0x7FFFFFFF); //error
				/*if(addr > 0x400)  //arm9 -> arm7 con crushed try to get at last some infos
				{
					swiWaitForVBlank();
					swiWaitForVBlank();
					senddebug32(addr);
					senddebug32(addr);
					senddebug32(addr);
					senddebug32(addr);
					senddebug32(addr);
					senddebug32(addr);
					senddebug32(addr);
					senddebug32(addr);
					senddebug32(addr);
					while(1);
				}*/
				senddebug32(addr);
				//senddebug32(val);
				//senddebug32(REG_IPC_FIFO_CR);
				break;
			}
			leaveCriticalSection(oldIME);
}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	ledBlink(0);
	readUserSettings();
	
	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();

	enableSound();

	REG_IPC_SYNC = 0;
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE;
	REG_IPC_FIFO_CR = IPC_FIFO_SEND_CLEAR | IPC_FIFO_ENABLE | IPC_FIFO_ERROR;


	//soundbuffA = malloc(32);
	//soundbuffB = malloc(32);

	SCHANNEL_REPEAT_POINT(4) = 0;
	SCHANNEL_LENGTH(4) = 8;

	SCHANNEL_REPEAT_POINT(5) = 0;
	SCHANNEL_LENGTH(5) = 8;

	bool ykeypp = false;
	while (true) {
		//sound alloc
		//0-3 matching gba
		//4-5 FIFO
		//ledBlink(1);
		//swiWaitForVBlank();
		if((REG_VCOUNT == callline) /*&& (REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)*/ && (REG_KEYXY & 0x1)) //X not pressed
		{
			REG_IPC_FIFO_TX = 0x3F00BEEF; //send cmd 0x3F00BEEF
			while(REG_VCOUNT == callline); //don't send 2 or more
		}
		if(!(REG_KEYXY & 0x2))
		{
			if(!ykeypp)
			{
				REG_IPC_FIFO_TX = 0x4200BEEF;
				while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
				int val2 = REG_IPC_FIFO_RX; //Value skip
				ykeypp = true;
			}
		}
		else
		{
			ykeypp = false;
		}

		while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
		{
#ifdef checkforerror
			if(REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
			{
				senddebug32(0x7FFFFFF0);
				//REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
			}
#endif
			int addr = (REG_IPC_FIFO_RX & ~0xC0000000/*todo*/); //addr + flags //flags 2 most upperen Bits dma = 0 u8 = 1 u16 = 2 u32 = 3
			while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
			int val = REG_IPC_FIFO_RX; //Value skip add for speedup
#ifdef checkforerror
			if(REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
			{
				senddebug32(0x7FFFFFF1);
				//REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
			}
#endif
			newvalwrite(addr,val);
#ifdef checkforerror
			if(REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
			{
				senddebug32(0x7FFFFFF2);
				//REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
			}
#endif
		}
	}
	return 0;
}

void updatevol()
{

	/*****************************************************/
	/*                  Update Voll                      */
	/*****************************************************/


	//Sound_chan = (Volume_Right * enabeled + Volume_Left * enabeled) * (Soundcnt(1,2,4))*static_for_max
	//DMA = (Soundcnt(1,2) * enabeled + Soundcnt(1,2) * enabeled) * sataic_for_max
	SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF) | ((( 1 + ((SOUNDCNT_H & 0x4) >> 2))*((SOUNDCNT_H & BIT(8)) >> 8) + ( 1 + ((SOUNDCNT_H & 0x4) >> 2))*((SOUNDCNT_H & BIT(9)) >> 9))*31);     //max:124
	SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF) | ((( 1 + ((SOUNDCNT_H & 0x8) >> 3))*((SOUNDCNT_H & BIT(12)) >> 12) + ( 1 + ((SOUNDCNT_H & 0x8) >> 3))*((SOUNDCNT_H & BIT(13)) >> 13))*31); //max:124
	int Vol = SOUNDCNT_H & 0x3;
	switch(Vol)
	{
	case 3:
	case 0:
		Vol = 1;
		break;
	case 1:
		Vol = 2;
		break;
	case 2:
		Vol = 4;
		break;
	}
	int Masterright = SOUNDCNT_L & 0x7;
	int Masterleft =  (SOUNDCNT_L << 4) & 0x7;
	SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(8)) >> 8) + Masterleft * ((SOUNDCNT_L & BIT(12)) >> 12) ) * Vol *2);  //max:112
	SCHANNEL_CR(1) = (SCHANNEL_CR(1) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(9)) >> 9) + Masterleft * ((SOUNDCNT_L & BIT(13)) >> 13) ) * Vol *2);  //max:112
	SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(10)) >> 10) + Masterleft * ((SOUNDCNT_L & BIT(14)) >> 14) ) * Vol *2);//max:112
	SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF) | ((Masterright * ((SOUNDCNT_L & BIT(11)) >> 11) + Masterleft * ((SOUNDCNT_L & BIT(15)) >> 15) ) * Vol *2);//max:112


    /*****************************************************/
	/*                  Update Panning                   */
	/*****************************************************/
	

	if(SOUNDCNT_H & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000);
	if(SOUNDCNT_H & BIT(8)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_H & BIT(8) && SOUNDCNT_H & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(4) & ~0xFF0000) | 0x400000; //same on both

	if(SOUNDCNT_H & BIT(13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000);
	if(SOUNDCNT_H & BIT(12)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_H & BIT(12) && SOUNDCNT_H & BIT(13)) SCHANNEL_CR(5) = (SCHANNEL_CR(5) & ~0xFF0000) | 0x400000; //same on both

	int right = SOUNDCNT_L & 7;
	int left = (SOUNDCNT_L << 4) & 7;
	int tempmixedvol1_4 = 0;
	if((left + right) != 0) //don't work
	{
		tempmixedvol1_4 = ((right*0x7F0000)/(right + left) & 0x7F0000);
	}
	
	if(SOUNDCNT_L & BIT(12)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(8)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(8) && SOUNDCNT_L & BIT(12)) SCHANNEL_CR(0) = (SCHANNEL_CR(0) & ~0xFF0000) | tempmixedvol1_4;

	if(SOUNDCNT_L & BIT(13)) SCHANNEL_CR(4) = (SCHANNEL_CR(1) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(9)) SCHANNEL_CR(4) = (SCHANNEL_CR(1) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(9) && SOUNDCNT_L & BIT(13)) SCHANNEL_CR(1) = (SCHANNEL_CR(1) & ~0xFF0000) | tempmixedvol1_4; 

	if(SOUNDCNT_L & BIT(14)) SCHANNEL_CR(4) = (SCHANNEL_CR(2) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(10)) SCHANNEL_CR(4) = (SCHANNEL_CR(2) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(10) && SOUNDCNT_L & BIT(14)) SCHANNEL_CR(2) = (SCHANNEL_CR(2) & ~0xFF0000) | tempmixedvol1_4;

	if(SOUNDCNT_L & BIT(15)) SCHANNEL_CR(4) = (SCHANNEL_CR(3) & ~0xFF0000);
	if(SOUNDCNT_L & BIT(11)) SCHANNEL_CR(4) = (SCHANNEL_CR(3) & ~0xFF0000) | 0x7F0000;
	if(SOUNDCNT_L & BIT(11) && SOUNDCNT_L & BIT(15)) SCHANNEL_CR(3) = (SCHANNEL_CR(3) & ~0xFF0000) | tempmixedvol1_4;

}
void updatetakt()
{
	//FIFO A
	if(tacktgeber_sound_FIFO_DMA_A == 0)
	{
		int seek;
		switch(TM0CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(4) = debugfr1 = (((-TM0CNT_L) << seek) & 0xFFFF) << 1;
	}
	else
	{
		int seek;
		switch(TM1CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(4) = debugfr1 = (((-TM1CNT_L) & 0xFFFF) << seek) << 1;
	}
	//FIFO B
	if(tacktgeber_sound_FIFO_DMA_B == 0)
	{
		int seek;
		switch(TM0CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(5) = debugfr2 = (((-TM0CNT_L) << seek) & 0xFFFF) << 1;
	}
	else
	{
		int seek;
		switch(TM1CNT_H & 0x3)
		{
			case 0:
				seek = 0;
				break;
			case 1:
				seek = 6;
				break;
			case 2:
				seek = 8;
				break;
			case 3:
				seek = 10;
				break;
		}
		SCHANNEL_TIMER(5) = debugfr2 = (((-TM1CNT_L) << seek) & 0xFFFF) << 1; //everything is 2 times faster than on ther gba here
	}
}
#ifdef nichtdef
void checkstart()
{
	//DMA 1
	if(DMA1DAD_L == 0x00A0 && DMA1DAD_H == 0x0400 && (DMA1CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA1SAD_L | (DMA1SAD_H << 16);
				//senddebug32(debugsrc1);
				dmaAtimerinter();
				dmaAtimerinter();
				SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 4, dmaAtimerinter);
				////senddebug32((-debugfr1) << 4);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 8;
				SCHANNEL_LENGTH(4) = 0;
			}
		}
		else
		{
			if(TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA1SAD_L | (DMA1SAD_H << 16);
				//senddebug32(debugsrc1);
				dmaAtimerinter();
				dmaAtimerinter();
				SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 4, dmaAtimerinter);
				//senddebug32((-debugfr1) << 4);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 8;
				SCHANNEL_LENGTH(4) = 0;
				////senddebug32(SCHANNEL_CR(4));
			}
		}
	}
	else
	{
		if(DMA1DAD_L == 0x00A4 && DMA1DAD_H == 0x0400 && (DMA1CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA1SAD_L | (DMA1SAD_H << 16);
					//senddebug32(debugsrc2);
					dmaBtimerinter();
					dmaBtimerinter();
					SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 4, dmaBtimerinter);
				//senddebug32((-debugfr2) << 4);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 8;
					SCHANNEL_LENGTH(5) = 0;
					////senddebug32(SCHANNEL_CR(5));
				}
			}
			else
			{
				if(TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA1SAD_L | (DMA1SAD_H << 16);
					//senddebug32(debugsrc2);
					dmaBtimerinter();
					dmaBtimerinter();
					SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 4, dmaBtimerinter);
				//senddebug32((-debugfr2) << 4);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 8;
					SCHANNEL_LENGTH(5) = 0;
					////senddebug32(SCHANNEL_CR(5));
				}
			}
		}

	}


	//DMA 2
	if(DMA2DAD_L == 0x00A0 && DMA2DAD_H == 0x0400 && (DMA2CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA2SAD_L | (DMA2SAD_H << 16);
				//senddebug32(debugsrc1);
				dmaAtimerinter();
				dmaAtimerinter();
				SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 4, dmaAtimerinter);
				//senddebug32((-debugfr1) << 4);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 8;
				SCHANNEL_LENGTH(4) = 0;
				////senddebug32(SCHANNEL_CR(5));
			}
		}
		else
		{
			if(TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA2SAD_L | (DMA2SAD_H << 16);
				//senddebug32(debugsrc1);
				dmaAtimerinter();
				dmaAtimerinter();
				SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 4, dmaAtimerinter);
				//senddebug32((-debugfr1) << 4);
			}
			else
			{
				SCHANNEL_CR(4) &= 0x80000000; //stop now
				dmaApart = 0;
				SCHANNEL_SOURCE(4) = soundbuffA;
				SCHANNEL_REPEAT_POINT(4) = 8;
				SCHANNEL_LENGTH(4) = 0;
				////senddebug32(SCHANNEL_CR(5));
			}
		}
	}
	else
	{
		if(DMA2DAD_L == 0x00A4 && DMA2DAD_H == 0x0400 && (DMA2CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA2SAD_L | (DMA2SAD_H << 16);
					//senddebug32(debugsrc2);
					dmaBtimerinter();
					dmaBtimerinter();
					SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 4, dmaBtimerinter);
				//senddebug32((-debugfr2) << 4);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 8;
					SCHANNEL_LENGTH(5) = 0;
					////senddebug32(SCHANNEL_CR(5));
				}
			}
			else
			{
				if(TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA2SAD_L | (DMA2SAD_H << 16);
					//senddebug32(debugsrc2);
					dmaBtimerinter();
					dmaBtimerinter();
					SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 4, dmaBtimerinter);
				//senddebug32((-debugfr2) << 4);
				}
				else
				{
					SCHANNEL_CR(5) &= 0x80000000; //stop now
					dmaApart = 0;
					SCHANNEL_SOURCE(5) = soundbuffB;
					SCHANNEL_REPEAT_POINT(5) = 8;
					SCHANNEL_LENGTH(5) = 0;
					////senddebug32(SCHANNEL_CR(5));
				}
			}
		}

	}



}
#endif

void checkstart()
{
	//DMA 1
	if(DMA1DAD_L == 0x00A0 && DMA1DAD_H == 0x0400 && (DMA1CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA1SAD_L | (DMA1SAD_H << 16);
				//senddebug32(debugsrc1);
				SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 4, dmaAtimerinter);
				////senddebug32((-debugfr1) << 4);
			}
		}
		else
		{
			if(TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA1SAD_L | (DMA1SAD_H << 16);
				//senddebug32(debugsrc1);
				SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 4, dmaAtimerinter);
				//senddebug32((-debugfr1) << 4);
			}
		}
	}
	else
	{
		if(DMA1DAD_L == 0x00A4 && DMA1DAD_H == 0x0400 && (DMA1CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA1SAD_L | (DMA1SAD_H << 16);
					//senddebug32(debugsrc2);
					SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 4, dmaBtimerinter);
				//senddebug32((-debugfr2) << 4);
				}
			}
			else
			{
				if(TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA1SAD_L | (DMA1SAD_H << 16);
					//senddebug32(debugsrc2);
					SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 4, dmaBtimerinter);
				//senddebug32((-debugfr2) << 4);
				}
			}
		}

	}


	//DMA 2
	if(DMA2DAD_L == 0x00A0 && DMA2DAD_H == 0x0400 && (DMA2CNT_H & 0x8000))
	{
		//FIFO A
		if(tacktgeber_sound_FIFO_DMA_A == 0)
		{
			if(TM0CNT_H & 0x80) //started timer
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA2SAD_L | (DMA2SAD_H << 16);
				//senddebug32(debugsrc1);
				SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 4, dmaAtimerinter);
				//senddebug32((-debugfr1) << 4);
			}
		}
		else
		{
			if(TM1CNT_H & 0x80)
			{
				//SCHANNEL_LENGTH(4) = 0xFFFFFFFF;
				debugsrc1 = DMA2SAD_L | (DMA2SAD_H << 16);
				//senddebug32(debugsrc1);
				SCHANNEL_CR(4) |= 0x80000000 |SOUND_REPEAT; //start now
				timerStart(0, ClockDivider_1,(-debugfr1) << 4, dmaAtimerinter);
				//senddebug32((-debugfr1) << 4);
			}
		}
	}
	else
	{
		if(DMA2DAD_L == 0x00A4 && DMA2DAD_H == 0x0400 && (DMA2CNT_H & 0x8000))
		{
			//FIFO B
			if(tacktgeber_sound_FIFO_DMA_B == 0)
			{
				if(TM0CNT_H & 0x80) //started timer
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA2SAD_L | (DMA2SAD_H << 16);
					//senddebug32(debugsrc2);
					SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 4, dmaBtimerinter);
				//senddebug32((-debugfr2) << 4);
				}
			}
			else
			{
				if(TM1CNT_H & 0x80)
				{
					//SCHANNEL_LENGTH(5) = 0xFFFFFFFF;
					debugsrc2 = DMA2SAD_L | (DMA2SAD_H << 16);
					//senddebug32(debugsrc2);
					SCHANNEL_CR(5) |= 0x80000000 |SOUND_REPEAT; //start now
					timerStart(1, ClockDivider_1,(-debugfr2) << 4, dmaBtimerinter);
				}
			}
		}

	}



}
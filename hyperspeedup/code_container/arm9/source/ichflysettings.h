//functions

#define arm9advsound
#define uppern_read_emulation //using POSIX ichflyfilestream FILESTREAM
#define ownfilebuffer //don't remove it if you want ichfly_dma_rom
//#define asmspeedup

//extra settings for ownfilebuffer
#define chucksizeinsec 1 //1,2,4,8
#define buffslots 255

#define chucksize 0x200*chucksizeinsec

//settings
#define gba_handel_IRQ_correct
#define HBlankdma //only if hblanc irq is on
#define forceHBlankirqs
#define advanced_irq_check
//#define priosound //better sound but worse performance
#define ichflytestkeypossibillity //so up and down and left and right can not be pressed at the same time crashes some games

//debug
#define usebuffedVcout
#define directcpu //rename anothercpu.cpp to anothercpu.h
#define unsave //save only work without directcpu
#define patch_VERSION //is set even if this is not defined
#define releas //non releas no more working 

#define capture_and_pars //scales up the pricture but cause 1 frame input lag and a lot of cpu time in Vblanc
//more settings for caputer
#define antyflicker //if this is not defined the Picture dose not cause a lot of cpu time but the image flickers
//#defin skipper //fram skiping
#include <math.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdlib.h>

#include "GBA.h"
#include "bios.h"
#include "GBAinline.h"
#include "Globals.h"

void triggerdmairq(u8 num) //tigger an dma irq if dma is free
{
	DMA_SRC(num) = 0x1100000; //don't care about that it is open by pu and not writeabel by the dma
	DMA_DEST(num) = 0x1200000; //don't care about that it is open by pu and not writeabel by the dma
	DMA_CR(num) = DMA_ENABLE | DMA_IRQ_REQ | DMA_START_NOW | DMA_16_BIT | DMA_SRC_INC | DMA_DST_INC | 1;
}
#include <math.h>
#include "GBA.h"
#include "bios.h"

#include "ds_dma.h"

void triggerdmairq(u8 num) //tigger an dma irq if dma is free
{
	DMA_SRC(num) = 0x1100000; //don't care about that it is open by pu and not writeabel by the dma
	DMA_DEST(num) = 0x1200000; //don't care about that it is open by pu and not writeabel by the dma
	DMA_CR(num) = DMA_ENABLE | DMA_IRQ_REQ | DMA_START_NOW | DMA_16_BIT | DMA_SRC_INC | DMA_DST_INC | 1;
}
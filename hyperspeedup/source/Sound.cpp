#include <nds.h>
#include <stdio.h>

void arm7dmareq()
{
	DMA0_SRC = REG_IPC_FIFO_RX;
	DMA0_DEST = REG_IPC_FIFO_RX;
	DMA0_CR = REG_IPC_FIFO_RX;
}
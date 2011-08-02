
#include <nds.h>

void workaroundwrite32(u32 zahl, u32* addr)
{
	*addr = zahl;
}

void workaroundwrit16(u16 zahl, u16* addr)
{
	*addr = zahl;
}

u32 workaroundread32( u32* addr)
{
	return *addr;
}

u16 workaroundread16( u16* addr)
{
	return *addr;
}
#include "GBAinlineforpu.h"


u32 CPUReadMemoryrealpu(u32 address);

u32 CPUReadMemorypu(u32 address)
 {
	return CPUReadMemoryrealpu(address);
 }

u32 CPUReadHalfWordrealpu(u32 address);

u32 CPUReadHalfWordpu(u32 address)
 {
	return CPUReadHalfWordrealpu(address);
 }
u8 CPUReadByterealpu(u32 address);

u8 CPUReadBytepu(u32 address)
{
	return CPUReadByterealpu(address);
}





void CPUWriteMemorypu(u32 address, u32 value);

void CPUWriteMemorypuextern(u32 address, u32 value)
 {
	CPUWriteMemorypu(address,value);
 }

void CPUWriteHalfWordpu(u32 address, u16 value);

void CPUWriteHalfWordpuextern(u32 address, u16 value)
 {
	CPUWriteHalfWordpu(address,value);
 }
void CPUWriteBytepu(u32 address, u8 b);

void CPUWriteBytepuextern(u32 address, u8 b)
{
	CPUWriteBytepu(address,b);
}

s32 CPUReadHalfWordrealpuSigned(u32 address);

s32 CPUReadHalfWordrealpuSignedoutline(u32 address)
 {
	return CPUReadHalfWordrealpuSigned(address);
 }

s8 CPUReadByteSignedpu(u32 address)
{
	return (s8)CPUReadByterealpu(address);
}
#include "GBAinline.h"

//#include "fatfileextract.h"

u32 CPUReadMemoryreal(u32 address);
 

u32 CPUReadMemory(u32 address)
 {
	return CPUReadMemoryreal(address);
 }

u16 CPUReadHalfWordSigned(u32 address);

s16 CPUReadHalfWordSignedoutline(u32 address)
 {
	return (s16)CPUReadHalfWordSigned(address);
 }

s8 CPUReadByteSigned(u32 address)
{
	return (s8)CPUReadBytereal(address);
}


u32 CPUReadHalfWord(u32 address)
 {
	return CPUReadHalfWordreal(address);
 }
u8 CPUReadBytereal(u32 address);

u8 CPUReadByte(u32 address)
{
	return CPUReadBytereal(address);
}








void CPUWriteMemory(u32 address, u32 value);

void CPUWriteMemoryextern(u32 address, u32 value)
 {
	CPUWriteMemory(address,value);
 }

void CPUWriteHalfWord(u32 address, u16 value);

void CPUWriteHalfWordextern(u32 address, u16 value)
 {
	CPUWriteHalfWord(address,value);
 }
void CPUWriteByte(u32 address, u8 b);

void CPUWriteByteextern(u32 address, u8 b)
{
	CPUWriteByte(address,b);
}






u8 ichfly_readu8(unsigned int pos);

u32 ichfly_readu8extern(unsigned int pos)
 {
	return ichfly_readu8(pos);
 }

u16 ichfly_readu16(unsigned int pos);

u16 ichfly_readu16extern(unsigned int pos)
 {
	return ichfly_readu16(pos);
 }
u32 ichfly_readu32(unsigned int pos);

u32 ichfly_readu32extern(unsigned int pos)
 {
	return ichfly_readu32(pos);
 }


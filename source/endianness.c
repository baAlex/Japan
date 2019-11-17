/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [endianness.c]
 - Alexander Brandt 2019

 https://www.ibm.com/developerworks/aix/library/au-endianc/index.html
-----------------------------*/

#include "endianness.h"
#include "common.h"


EXPORT inline int64_t EndianReverseI64(int64_t value)
{
	union { int64_t i; uint64_t u; } conv = {.i = value};
	conv.u = EndianReverseU64(conv.u);
	return conv.i;
}

EXPORT inline int32_t EndianReverseI32(int32_t value)
{
	union { int32_t i; uint32_t u; } conv = {.i = value};
	conv.u = EndianReverseU32(conv.u);
	return conv.i;
}

EXPORT inline int16_t EndianReverseI16(int16_t value)
{
	union { int16_t i; uint16_t u; } conv = {.i = value};
	conv.u = EndianReverseU16(conv.u);
	return conv.i;
}

EXPORT inline int64_t EndianToI64(int64_t value, enum Endianness from, enum Endianness to)
{
	union { int64_t i; uint64_t u; } conv = {.i = value};
	conv.u = EndianToU64(conv.u, from, to);
	return conv.i;
}

EXPORT inline int32_t EndianToI32(int32_t value, enum Endianness from, enum Endianness to)
{
	union { int32_t i; uint32_t u; } conv = {.i = value};
	conv.u = EndianToU32(conv.u, from, to);
	return conv.i;
}

EXPORT inline int16_t EndianToI16(int16_t value, enum Endianness from, enum Endianness to)
{
	union { int16_t i; uint16_t u; } conv = {.i = value};
	conv.u = EndianToU16(conv.u, from, to);
	return conv.i;
}


/*-----------------------------

 EndianSystem()
-----------------------------*/
EXPORT inline enum Endianness EndianSystem()
{
	int16_t i = 1;
	int8_t* p = (int8_t*)&i;

	if (p[0] != 1)
		return ENDIAN_BIG;

	return ENDIAN_LITTLE;
}


/*-----------------------------

 EndianReverse
-----------------------------*/
EXPORT inline uint64_t EndianReverseU64(uint64_t value)
{
	uint8_t b1 = (uint8_t)((uint64_t)value & 0xFF);
	uint8_t b2 = (uint8_t)((uint64_t)value >> 8) & 0xFF;
	uint8_t b3 = (uint8_t)((uint64_t)value >> 16) & 0xFF;
	uint8_t b4 = (uint8_t)((uint64_t)value >> 24) & 0xFF;
	uint8_t b5 = (uint8_t)((uint64_t)value >> 32) & 0xFF;
	uint8_t b6 = (uint8_t)((uint64_t)value >> 40) & 0xFF;
	uint8_t b7 = (uint8_t)((uint64_t)value >> 48) & 0xFF;
	uint8_t b8 = (uint8_t)((uint64_t)value >> 56) & 0xFF;

	return ((uint64_t)b1 << 56) | ((uint64_t)b2 << 48) | ((uint64_t)b3 << 40) | ((uint64_t)b4 << 32) |
	       ((uint64_t)b5 << 24) | ((uint64_t)b6 << 16) | ((uint64_t)b7 << 8) | b8;
}

EXPORT inline uint32_t EndianReverseU32(uint32_t value)
{
	uint8_t b1 = (uint8_t)((uint32_t)value & 0xFF);
	uint8_t b2 = (uint8_t)((uint32_t)value >> 8) & 0xFF;
	uint8_t b3 = (uint8_t)((uint32_t)value >> 16) & 0xFF;
	uint8_t b4 = (uint8_t)((uint32_t)value >> 24) & 0xFF;

	return ((uint32_t)b1 << 24) | ((uint32_t)b2 << 16) | ((uint32_t)b3 << 8) | b4;
}

EXPORT inline uint16_t EndianReverseU16(uint16_t value)
{
	uint8_t b1 = (uint8_t)((uint16_t)value & 0xFF);
	uint8_t b2 = (uint8_t)((uint16_t)value >> 8);

	return (uint16_t)(((uint16_t)b1 << 8) | b2);
}


/*-----------------------------

 EndianTo
-----------------------------*/
EXPORT uint64_t EndianToU64(uint64_t value, enum Endianness from, enum Endianness to)
{
	to = (to == ENDIAN_SYSTEM) ? EndianSystem() : to;
	from = (from == ENDIAN_SYSTEM) ? EndianSystem() : from;

	if (from == to)
		return value;

	return EndianReverseU64(value);
}

EXPORT uint32_t EndianToU32(uint32_t value, enum Endianness from, enum Endianness to)
{
	to = (to == ENDIAN_SYSTEM) ? EndianSystem() : to;
	from = (from == ENDIAN_SYSTEM) ? EndianSystem() : from;

	if (from == to)
		return value;

	return EndianReverseU32(value);
}

EXPORT uint16_t EndianToU16(uint16_t value, enum Endianness from, enum Endianness to)
{
	to = (to == ENDIAN_SYSTEM) ? EndianSystem() : to;
	from = (from == ENDIAN_SYSTEM) ? EndianSystem() : from;

	if (from == to)
		return value;

	return EndianReverseU16(value);
}

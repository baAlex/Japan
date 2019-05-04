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
#include "local.h"


/*-----------------------------

 EndianSystem()
-----------------------------*/
export inline enum Endianness EndianSystem()
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
export inline uint64_t EndianReverse_64(uint64_t value)
{
	uint8_t b1 = value & 0xFF;
	uint8_t b2 = (value >> 8) & 0xFF;
	uint8_t b3 = (value >> 16) & 0xFF;
	uint8_t b4 = (value >> 24) & 0xFF;
	uint8_t b5 = (value >> 32) & 0xFF;
	uint8_t b6 = (value >> 40) & 0xFF;
	uint8_t b7 = (value >> 48) & 0xFF;
	uint8_t b8 = (value >> 56) & 0xFF;

	return ((uint64_t)b1 << 56) | ((uint64_t)b2 << 48) | ((uint64_t)b3 << 40) | ((uint64_t)b4 << 32) |
		   ((uint64_t)b5 << 24) | ((uint64_t)b6 << 16) | ((uint64_t)b7 << 8) | b8;
}

export inline uint32_t EndianReverse_32(uint32_t value)
{
	uint8_t b1 = value & 0xFF;
	uint8_t b2 = (value >> 8) & 0xFF;
	uint8_t b3 = (value >> 16) & 0xFF;
	uint8_t b4 = (value >> 24) & 0xFF;

	return ((uint32_t)b1 << 24) | ((uint32_t)b2 << 16) | ((uint32_t)b3 << 8) | b4;
}

export inline uint16_t EndianReverse_16(uint16_t value)
{
	uint8_t b1 = value & 0xFF;
	uint8_t b2 = (value >> 8);

	return (uint16_t)(((uint16_t)b1 << 8) | b2);
}


/*-----------------------------

 EndianBigToSystem
-----------------------------*/
export inline uint64_t EndianBigToSystem_64(uint64_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_BIG)
		return value;

	return EndianReverse_64(value);
}

export inline uint32_t EndianBigToSystem_32(uint32_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_BIG)
		return value;

	return EndianReverse_32(value);
}

export inline uint16_t EndianBigToSystem_16(uint16_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_BIG)
		return value;

	return EndianReverse_16(value);
}


/*-----------------------------

 EndianLittleToSystem
-----------------------------*/
export inline uint64_t EndianLittleToSystem_64(uint64_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_LITTLE)
		return value;

	return EndianReverse_64(value);
}

export inline uint32_t EndianLittleToSystem_32(uint32_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_LITTLE)
		return value;

	return EndianReverse_32(value);
}

export inline uint16_t EndianLittleToSystem_16(uint16_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_LITTLE)
		return value;

	return EndianReverse_16(value);
}


/*-----------------------------

 EndianSystemToBig
-----------------------------*/
export inline uint64_t EndianSystemToBig_64(uint64_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_BIG)
		return value;

	return EndianReverse_64(value);
}

export inline uint32_t EndianSystemToBig_32(uint32_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_BIG)
		return value;

	return EndianReverse_32(value);
}

export inline uint16_t EndianSystemToBig_16(uint16_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_BIG)
		return value;

	return EndianReverse_16(value);
}


/*-----------------------------

 EndianSystemToLittle
-----------------------------*/
export inline uint64_t EndianSystemToLittle_64(uint64_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_LITTLE)
		return value;

	return EndianReverse_64(value);
}

export inline uint32_t EndianSystemToLittle_32(uint32_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_LITTLE)
		return value;

	return EndianReverse_32(value);
}

export inline uint16_t EndianSystemToLittle_16(uint16_t value, enum Endianness system)
{
	system = (system == ENDIAN_UNKNOWN) ? EndianSystem() : system;

	if (system == ENDIAN_LITTLE)
		return value;

	return EndianReverse_16(value);
}

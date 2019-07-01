/*-----------------------------

 [endianness.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.0"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 0
#endif

#ifndef ENDIANNESS_H
#define ENDIANNESS_H

	#include <stdint.h>

	enum Endianness
	{
		ENDIAN_SYSTEM = -1,
		ENDIAN_LITTLE = 0,
		ENDIAN_BIG
	};

	enum Endianness EndianSystem();

	uint64_t EndianReverse_64(uint64_t value);
	uint32_t EndianReverse_32(uint32_t value);
	uint16_t EndianReverse_16(uint16_t value);

	uint64_t EndianTo_64(uint64_t value, enum Endianness from, enum Endianness to);
	uint32_t EndianTo_32(uint32_t value, enum Endianness from, enum Endianness to);
	uint16_t EndianTo_16(uint16_t value, enum Endianness from, enum Endianness to);

#endif

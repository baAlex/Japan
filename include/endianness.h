/*-----------------------------

 [endianness.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef ENDIANNESS_H
#define ENDIANNESS_H

	#include <stdint.h>

	enum Endianness
	{
		ENDIAN_UNKNOWN = -1,
		ENDIAN_LITTLE = 0,
		ENDIAN_BIG
	};

	enum Endianness EndianSystem();

	uint64_t EndianReverse_64(uint64_t value);
	uint32_t EndianReverse_32(uint32_t value);
	uint16_t EndianReverse_16(uint16_t value);

	uint64_t EndianBigToSystem_64(uint64_t value, enum Endianness system);
	uint32_t EndianBigToSystem_32(uint32_t value, enum Endianness system);
	uint16_t EndianBigToSystem_16(uint16_t value, enum Endianness system);

	uint64_t EndianLittleToSystem_64(uint64_t value, enum Endianness system);
	uint32_t EndianLittleToSystem_32(uint32_t value, enum Endianness system);
	uint16_t EndianLittleToSystem_16(uint16_t value, enum Endianness system);

	uint64_t EndianSystemToBig_64(uint64_t value, enum Endianness system);
	uint32_t EndianSystemToBig_32(uint32_t value, enum Endianness system);
	uint16_t EndianSystemToBig_16(uint16_t value, enum Endianness system);

	uint64_t EndianSystemToLittle_64(uint64_t value, enum Endianness system);
	uint32_t EndianSystemToLittle_32(uint32_t value, enum Endianness system);
	uint16_t EndianSystemToLittle_16(uint16_t value, enum Endianness system);

	uint64_t EndianTo_64(uint64_t value, enum Endianness from, enum Endianness to);
	uint32_t EndianTo_32(uint32_t value, enum Endianness from, enum Endianness to);
	uint16_t EndianTo_16(uint16_t value, enum Endianness from, enum Endianness to);

#endif

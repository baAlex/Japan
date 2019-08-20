/*-----------------------------

 [endianness.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.1"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 1
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

	uint64_t EndianReverse_u64(uint64_t value);
	uint32_t EndianReverse_u32(uint32_t value);
	uint16_t EndianReverse_u16(uint16_t value);

	int64_t EndianReverse_i64(int64_t value);
	int32_t EndianReverse_i32(int32_t value);
	int16_t EndianReverse_i16(int16_t value);

	#define EndianReverse(value) _Generic((value), \
		uint64_t: EndianReverse_u64, \
		uint32_t: EndianReverse_u32, \
		uint16_t: EndianReverse_u16, \
		int64_t: EndianReverse_i64, \
		int32_t: EndianReverse_i32, \
		int16_t: EndianReverse_i16, \
		default: EndianReverse_i32 \
	)(value)

	uint64_t EndianTo_u64(uint64_t value, enum Endianness from, enum Endianness to);
	uint32_t EndianTo_u32(uint32_t value, enum Endianness from, enum Endianness to);
	uint16_t EndianTo_u16(uint16_t value, enum Endianness from, enum Endianness to);

	int64_t EndianTo_i64(int64_t value, enum Endianness from, enum Endianness to);
	int32_t EndianTo_i32(int32_t value, enum Endianness from, enum Endianness to);
	int16_t EndianTo_i16(int16_t value, enum Endianness from, enum Endianness to);

	#define EndianTo(value, from, to) _Generic((value), \
		uint64_t: EndianTo_u64, \
		uint32_t: EndianTo_u32, \
		uint16_t: EndianTo_u16, \
		int64_t: EndianTo_i64, \
		int32_t: EndianTo_i32, \
		int16_t: EndianTo_i16, \
		default: EndianTo_i32 \
	)(value, from, to)

#endif

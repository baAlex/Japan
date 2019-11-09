/*-----------------------------

 [endianness.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#if defined(EXPORT_SYMBOLS) && defined(_WIN32)
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP // Whitespace
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

	DLL_EXP enum Endianness EndianSystem();

	DLL_EXP uint64_t EndianReverse_u64(uint64_t value);
	DLL_EXP uint32_t EndianReverse_u32(uint32_t value);
	DLL_EXP uint16_t EndianReverse_u16(uint16_t value);

	DLL_EXP int64_t EndianReverse_i64(int64_t value);
	DLL_EXP int32_t EndianReverse_i32(int32_t value);
	DLL_EXP int16_t EndianReverse_i16(int16_t value);

	DLL_EXP uint64_t EndianTo_u64(uint64_t value, enum Endianness from, enum Endianness to);
	DLL_EXP uint32_t EndianTo_u32(uint32_t value, enum Endianness from, enum Endianness to);
	DLL_EXP uint16_t EndianTo_u16(uint16_t value, enum Endianness from, enum Endianness to);

	DLL_EXP int64_t EndianTo_i64(int64_t value, enum Endianness from, enum Endianness to);
	DLL_EXP int32_t EndianTo_i32(int32_t value, enum Endianness from, enum Endianness to);
	DLL_EXP int16_t EndianTo_i16(int16_t value, enum Endianness from, enum Endianness to);


	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

	#define EndianReverse(value) _Generic((value), \
		uint64_t: EndianReverse_u64, \
		uint32_t: EndianReverse_u32, \
		uint16_t: EndianReverse_u16, \
		int64_t: EndianReverse_i64, \
		int32_t: EndianReverse_i32, \
		int16_t: EndianReverse_i16, \
		default: EndianReverse_i32 \
	)(value)

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

#endif

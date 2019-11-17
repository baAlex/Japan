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
#define JAPAN_API __declspec(dllexport)
#else
#define JAPAN_API // Whitespace
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

	JAPAN_API enum Endianness EndianSystem();

	JAPAN_API uint64_t EndianReverseU64(uint64_t value);
	JAPAN_API uint32_t EndianReverseU32(uint32_t value);
	JAPAN_API uint16_t EndianReverseU16(uint16_t value);

	JAPAN_API int64_t EndianReverseI64(int64_t value);
	JAPAN_API int32_t EndianReverseI32(int32_t value);
	JAPAN_API int16_t EndianReverseI16(int16_t value);

	JAPAN_API uint64_t EndianToU64(uint64_t value, enum Endianness from, enum Endianness to);
	JAPAN_API uint32_t EndianToU32(uint32_t value, enum Endianness from, enum Endianness to);
	JAPAN_API uint16_t EndianToU16(uint16_t value, enum Endianness from, enum Endianness to);

	JAPAN_API int64_t EndianToI64(int64_t value, enum Endianness from, enum Endianness to);
	JAPAN_API int32_t EndianToI32(int32_t value, enum Endianness from, enum Endianness to);
	JAPAN_API int16_t EndianToI16(int16_t value, enum Endianness from, enum Endianness to);


	#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

	#define EndianReverse(value) _Generic((value), \
		uint64_t: EndianReverseU64, \
		uint32_t: EndianReverseU32, \
		uint16_t: EndianReverseU16, \
		int64_t: EndianReverseI64, \
		int32_t: EndianReverseI32, \
		int16_t: EndianReverseI16, \
		default: EndianReverseI32 \
	)(value)

	#define EndianTo(value, from, to) _Generic((value), \
		uint64_t: EndianToU64, \
		uint32_t: EndianToU32, \
		uint16_t: EndianToU16, \
		int64_t: EndianToI64, \
		int32_t: EndianToI32, \
		int16_t: EndianToI16, \
		default: EndianToI32 \
	)(value, from, to)

	 #endif

#endif

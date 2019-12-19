/*-----------------------------

 [japan-endianness.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif


#ifndef JA_ENDIANNESS_H
#define JA_ENDIANNESS_H

#include <stdint.h>

enum jaEndianness
{
	ENDIAN_SYSTEM = -1,
	ENDIAN_LITTLE = 0,
	ENDIAN_BIG
};

JA_EXPORT enum jaEndianness jaEndianSystem();

JA_EXPORT uint64_t jaEndianReverseU64(uint64_t value);
JA_EXPORT uint32_t jaEndianReverseU32(uint32_t value);
JA_EXPORT uint16_t jaEndianReverseU16(uint16_t value);

JA_EXPORT int64_t jaEndianReverseI64(int64_t value);
JA_EXPORT int32_t jaEndianReverseI32(int32_t value);
JA_EXPORT int16_t jaEndianReverseI16(int16_t value);

JA_EXPORT uint64_t jaEndianToU64(uint64_t value, enum jaEndianness from, enum jaEndianness to);
JA_EXPORT uint32_t jaEndianToU32(uint32_t value, enum jaEndianness from, enum jaEndianness to);
JA_EXPORT uint16_t jaEndianToU16(uint16_t value, enum jaEndianness from, enum jaEndianness to);

JA_EXPORT int64_t jaEndianToI64(int64_t value, enum jaEndianness from, enum jaEndianness to);
JA_EXPORT int32_t jaEndianToI32(int32_t value, enum jaEndianness from, enum jaEndianness to);
JA_EXPORT int16_t jaEndianToI16(int16_t value, enum jaEndianness from, enum jaEndianness to);

#define jaEndianReverse(value)\
	_Generic((value),\
		uint64_t : jaEndianReverseU64,\
		uint32_t : jaEndianReverseU32,\
		uint16_t : jaEndianReverseU16,\
		int64_t : jaEndianReverseI64,\
		int32_t : jaEndianReverseI32,\
		int16_t : jaEndianReverseI16,\
		default : jaEndianReverseI32\
	)(value)

#define jaEndianTo(value, from, to)\
	_Generic((value),\
		uint64_t : jaEndianToU64,\
		uint32_t : jaEndianToU32,\
		uint16_t : jaEndianToU16,\
		int64_t : jaEndianToI64,\
		int32_t : jaEndianToI32,\
		int16_t : jaEndianToI16,\
		default : jaEndianToI32\
	)(value, from, to)

#endif

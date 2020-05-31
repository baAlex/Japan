/*-----------------------------

 [japan-endianness.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_ENDIANNESS_H
#define JAPAN_ENDIANNESS_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include <stdint.h>

enum jaEndianness
{
	JA_ENDIAN_SYSTEM = -1,
	JA_ENDIAN_LITTLE = 0,
	JA_ENDIAN_BIG
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

#if __STDC_VERSION__ == 201112L

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

#endif

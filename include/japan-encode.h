/*-----------------------------

 [japan-encode.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_ENCODE_H
#define JAPAN_ENCODE_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include <stddef.h>
#include <stdint.h>

#define JA_ASCII_NULL 0x00
#define JA_ASCII_EOL 0x0A // Aka LF

#define JA_UTF8_NULL 0x00
#define JA_UTF8_EOL 0x0A // Aka LF

JA_EXPORT int jaASCIIValidateUnit(uint8_t c);
JA_EXPORT int jaASCIIValidateString(const uint8_t* string, size_t bytes, size_t* out_bytes);

JA_EXPORT size_t jaUTF8UnitLength(uint8_t c); // A return of >3 means an encoding error in 'c'
JA_EXPORT int jaUTF8ValidateUnit(const uint8_t* c, size_t bytes, uint32_t* out_code);
JA_EXPORT int jaUTF8ValidateString(const uint8_t* string, size_t bytes, size_t* out_bytes, size_t* out_units);

#endif

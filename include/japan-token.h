/*-----------------------------

 [japan-token.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef JAPAN_TOKEN_H
#define JAPAN_TOKEN_H

#ifndef JA_WIP
#warning "LibJapan here!, be caution, this module is unstable"
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

#include "japan-status.h"
#include "japan-string.h"

struct jaTokenizer;

struct jaToken
{
	uint8_t* string;

	size_t line_number;
	size_t unit_number;
	size_t byte_offset;
};

JA_EXPORT struct jaTokenizer* jaTokenizerCreate(const uint8_t* string, size_t n, enum jaEncode,
                                                const uint32_t* separators);
JA_EXPORT void jaTokenizerDelete(struct jaTokenizer*);

JA_EXPORT struct jaToken* jaTokenize(struct jaTokenizer*, struct jaStatus* st);

#endif

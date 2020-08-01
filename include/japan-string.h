/*-----------------------------

 [japan-string.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef JAPAN_STRING_H
#define JAPAN_STRING_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct jaTokenizer;

struct jaTokenEnd
{
	// TODO: I'm sure that Unicode define a criteria on
	// what breaks and what not. It seems that everything
	// that is not alphanumeric breaks, except a underscore

	bool null : 1;
	bool whitespace : 1; // Spaces and tab
	bool new_line : 1;   // LF
	bool full_stop : 1;
	bool comma : 1;
	bool semicolon : 1;
	bool quotation : 1; // The English ones, single and double
	bool hyphen : 1;
};

struct jaToken
{
	uint8_t* string;
	uint8_t* end_string;

	size_t line_number;
	size_t unit_number;
	size_t byte_offset;

	struct jaTokenEnd end;
};

JA_EXPORT int jaASCIIValidateUnit(uint8_t byte);
JA_EXPORT int jaASCIIValidateString(const uint8_t* string, size_t n, size_t* out_bytes);

JA_EXPORT size_t jaUTF8UnitLength(uint8_t head_byte);
JA_EXPORT int jaUTF8ValidateUnit(const uint8_t* byte, size_t n, size_t* out_unit_len, uint32_t* out_unit_code);
JA_EXPORT int jaUTF8ValidateString(const uint8_t* string, size_t n, size_t* out_bytes, size_t* out_units);

JA_EXPORT struct jaTokenizer* jaASCIITokenizerCreate(const uint8_t* string, size_t n);
JA_EXPORT struct jaTokenizer* jaUTF8TokenizerCreate(const uint8_t* string, size_t n);
JA_EXPORT void jaTokenizerDelete(struct jaTokenizer*);

JA_EXPORT int jaTokenize(struct jaTokenizer*, struct jaToken* out_token);

#endif

/*-----------------------------

 [japan-token.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef JAPAN_TOKEN_H
#define JAPAN_TOKEN_H

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
#include <stdio.h>

#include "japan-status.h"
#include "japan-string.h"

#define JA_DELIMITER_NULL ((uint64_t)1)
#define JA_DELIMITER_WHITESPACE ((uint64_t)1 << 1) // Spaces and tab
#define JA_DELIMITER_NEW_LINE ((uint64_t)1 << 2)   // LF, discards CR
#define JA_DELIMITER_EXCLAMATION ((uint64_t)1 << 3)
#define JA_DELIMITER_QUOTATION ((uint64_t)1 << 4) // Aka: English double neutral quotation mark
#define JA_DELIMITER_HASH ((uint64_t)1 << 5)      // Unicode: Number Sign
#define JA_DELIMITER_DOLLAR ((uint64_t)1 << 6)
#define JA_DELIMITER_PERCENT ((uint64_t)1 << 7)
#define JA_DELIMITER_AMPERSAND ((uint64_t)1 << 8)
#define JA_DELIMITER_APOSTHOPHE ((uint64_t)1 << 9) // Aka: English simple neutral quotation mark
#define JA_DELIMITER_PARENTHESIS_L ((uint64_t)1 << 10)
#define JA_DELIMITER_PARENTHESIS_R ((uint64_t)1 << 11)
#define JA_DELIMITER_ASTERISK ((uint64_t)1 << 12)
#define JA_DELIMITER_PLUS ((uint64_t)1 << 13)
#define JA_DELIMITER_COMMA ((uint64_t)1 << 14)
#define JA_DELIMITER_MINUS ((uint64_t)1 << 15) // Unicode: Hyphen-minus
#define JA_DELIMITER_FULL_STOP ((uint64_t)1 << 16)
#define JA_DELIMITER_SLASH ((uint64_t)1 << 17) // Unicode: Solidus-slash
#define JA_DELIMITER_COLON ((uint64_t)1 << 18)
#define JA_DELIMITER_SEMICOLON ((uint64_t)1 << 19)
#define JA_DELIMITER_LESS_THAN ((uint64_t)1 << 20)
#define JA_DELIMITER_EQUALS ((uint64_t)1 << 21)
#define JA_DELIMITER_GREATER_THAN ((uint64_t)1 << 22)
#define JA_DELIMITER_QUESTION ((uint64_t)1 << 23)
#define JA_DELIMITER_AT ((uint64_t)1 << 24) // Unicode: Commercial at
#define JA_DELIMITER_SQUARE_BRACKET_L ((uint64_t)1 << 25)
#define JA_DELIMITER_BACKSLASH ((uint64_t)1 << 26) // Unicode: Reverse solidus-backslash
#define JA_DELIMITER_SQUARE_BRACKET_R ((uint64_t)1 << 27)
#define JA_DELIMITER_ACCENT ((uint64_t)1 << 28) // Unicode: Circumflex accent
#define JA_DELIMITER_GRAVE_ACCENT ((uint64_t)1 << 29)
#define JA_DELIMITER_CURLY_BRACKET_L ((uint64_t)1 << 30)
#define JA_DELIMITER_VERTICAL_LINE ((uint64_t)1 << 31)
#define JA_DELIMITER_CURLY_BRACKET_R ((uint64_t)1 << 32)
#define JA_DELIMITER_TILDE ((uint64_t)1 << 33)

struct jaTokenizer;

struct jaToken
{
	uint8_t* string;

	size_t line_number;
	size_t unit_number;
	size_t byte_offset;

	// Delimiters:

	uint8_t* start_string;
	uint8_t* end_string;

	uint64_t end_delimiters;   // JA_DELIMITER_X
	uint64_t start_delimiters; // JA_DELIMITER_X
};

JA_EXPORT struct jaTokenizer* jaTokenizerCreateString(enum jaStringEncode, const uint8_t* string, size_t n);
JA_EXPORT struct jaTokenizer* jaTokenizerCreateFile(enum jaStringEncode, FILE* file);
JA_EXPORT void jaTokenizerDelete(struct jaTokenizer*);

JA_EXPORT struct jaToken* jaTokenize(struct jaTokenizer*, struct jaStatus* st);

#endif

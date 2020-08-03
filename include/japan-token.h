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

#include "japan-status.h"

struct jaTokenizer;

struct jaTokenEnd
{
	bool null : 1;
	bool whitespace : 1; // Spaces and tab
	bool new_line : 1;   // LF, discards CR

	bool exclamation : 1;
	bool quotation : 1;   // Aka: English double neutral quotation mark
	bool number_sign : 1; // Aka: Hash(tag)
	bool dollar_sign : 1;
	bool percent_sign : 1;
	bool ampersand : 1;
	bool apostrophe : 1; // Aka: English simple neutral quotation mark
	bool left_parenthesis : 1;
	bool right_parenthesis : 1;
	bool asterisk : 1;
	bool plus_sign : 1;
	bool comma : 1;
	bool minus_sign : 1; // Unicode name: Hyphen-minus
	bool full_stop : 1;
	bool slash : 1; // Unicode name: Solidus-slash
	bool colon : 1;
	bool semicolon : 1;
	bool less_than_sign : 1;
	bool equals_sign : 1;
	bool greater_than_sign : 1;
	bool question_mark : 1;
	bool at_sign : 1; // Unicode name: Commercial at
	bool left_square_bracket : 1;
	bool backslash : 1; // Unicode name: Reverse solidus-backslash
	bool right_square_bracket : 1;
	bool accent : 1; // Unicode name: Circumflex accent
	bool grave_accent : 1;
	bool left_curly_bracket : 1;
	bool vertical_line : 1;
	bool right_curly_bracket : 1;
	bool tilde : 1;
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

JA_EXPORT struct jaTokenizer* jaASCIITokenizerCreate(const uint8_t* string, size_t n);
JA_EXPORT struct jaTokenizer* jaUTF8TokenizerCreate(const uint8_t* string, size_t n);
JA_EXPORT void jaTokenizerDelete(struct jaTokenizer*);

JA_EXPORT int jaTokenize(struct jaTokenizer*, struct jaToken* out_token, struct jaStatus* st);

#endif

/*-----------------------------

 [token/private.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef JA_TOKEN_PRIVATE_H
#define JA_TOKEN_PRIVATE_H

#include <stdlib.h>
#include <string.h>

#include "japan-token.h"

#include "japan-buffer.h"
#include "japan-status.h"
#include "japan-string.h"


#define FILE_BUFFER_LEN 256


struct jaTokenizer
{
	// Set at creation:
	enum jaStringEncode encode;

	const uint8_t* input;
	const uint8_t* input_end;

	FILE* file;

	struct jaStatus st;

	// Zero at creation:
	struct jaBuffer token_buffer;
	struct jaBuffer start_buffer;
	struct jaBuffer end_buffer;

	size_t line_number;        // A internal copy of what the user receives
	size_t unit_number;        // "
	size_t byte_offset;        // "
	uint64_t end_delimiters;   // "
	uint64_t start_delimiters; // "

	struct jaToken user;
};

uint64_t TranslateEnds(uint32_t code);
int BufferAppendByte(struct jaBuffer* buffer, size_t* cursor, uint8_t data, struct jaStatus* st);
int BufferAppend(struct jaBuffer* buffer, size_t* cursor, const void* data, size_t n, struct jaStatus* st);

int ASCIITokenizer(struct jaTokenizer* state);
int ASCIIFileTokenizer(struct jaTokenizer* state);

int UTF8Tokenizer(struct jaTokenizer* state);
int UTF8FileTokenizer(struct jaTokenizer* state);

#endif

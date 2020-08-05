/*-----------------------------

 [token/private.h]
 - Alexander Brandt 2020
-----------------------------*/

#ifndef JA_TOKEN_PRIVATE_H
#define JA_TOKEN_PRIVATE_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "japan-token.h"

#include "japan-buffer.h"
#include "japan-status.h"
#include "japan-string.h"

struct jaTokenizer
{
	// Set at creation:
	int (*callback)(struct jaTokenizer*);
	const uint8_t* input;
	const uint8_t* input_end;

	struct jaStatus st;

	// Zero at creation:
	struct jaBuffer token_buffer;
	struct jaBuffer end_buffer;

	size_t line_number; // A internal copy of what the user receives
	size_t unit_number; // "
	size_t byte_offset; // "
	uint64_t end;       // "

	struct jaToken user;
};

uint64_t TranslateEnds(uint32_t code);
int BufferAppendByte(struct jaBuffer* buffer, size_t* cursor, uint8_t data, struct jaStatus* st);
int BufferAppend(struct jaBuffer* buffer, size_t* cursor, const void* data, size_t n, struct jaStatus* st);

int UTF8Tokenizer(struct jaTokenizer* state);
int ASCIITokenizer(struct jaTokenizer* state);

#endif

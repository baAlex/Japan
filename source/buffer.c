/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [buffer.c]
 - Alexander Brandt 2019-2020
-----------------------------*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "japan-buffer.h"


inline void jaBufferClean(struct jaBuffer* buffer)
{
	if (buffer->data != NULL)
		free(buffer->data);

	buffer->data = NULL;
	buffer->size = 0;
}


int jaBufferCopy(struct jaBuffer* dest, const struct jaBuffer* org)
{
	if (org->size == 0)
		return 0;

	if (dest->size < org->size)
	{
		if (jaBufferResize(dest, org->size) == NULL)
			return 1;
	}

	memcpy(dest->data, org->data, org->size);
	return 0;
}


void* jaBufferResize(struct jaBuffer* buffer, size_t new_size)
{
	void* old_data = buffer->data;
	size_t old_size = buffer->size;

	if (new_size != 0 && (new_size > buffer->size || (new_size * 2) < (buffer->size / 2)))
	{
		buffer->size = new_size * 2;

		if ((buffer->data = realloc(buffer->data, buffer->size)) == NULL)
		{
			buffer->data = old_data;
			buffer->size = old_size;
			return NULL;
		}
	}

	if (old_size != buffer->size)
		JA_DEBUG_PRINT("(jaBufferResize) required %zu bytes, buffer size: %zu -> %zu bytes\n", new_size, old_size,
		               buffer->size);

	return buffer->data;
}


void* jaBufferResizeZero(struct jaBuffer* buffer, size_t new_size)
{
	size_t old_size = buffer->size;

	jaBufferResize(buffer, new_size);

	if (buffer->size > old_size)
		memset(((uint8_t*)buffer->data + old_size), 0, (buffer->size - old_size));

	return buffer;
}

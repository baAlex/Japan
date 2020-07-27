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

 [encode.c]
 - Alexander Brandt 2020

https://en.wikipedia.org/wiki/Ascii
https://en.wikipedia.org/wiki/UTF-8
-----------------------------*/

#include "japan-encode.h"


inline int jaASCIIValidateUnit(uint8_t c)
{
	return (c <= 127) ? 0 : 1;
}


int jaASCIIValidateString(const uint8_t* string, size_t bytes, size_t* out_bytes)
{
	size_t lenght = 0;

	for (const uint8_t* string_end = (string + bytes); string < string_end; string++)
	{
		if (jaASCIIValidateUnit(*string) != 0)
			break;

		lenght += 1;

		if (*string == JA_ASCII_NULL)
		{
			if (out_bytes != NULL)
				*out_bytes = lenght; // Counts NULL as part of the string

			return 0;
		}
	}

	if (out_bytes != NULL)
		*out_bytes = lenght; // Valid bytes so far

	return 1;
}


// 🌏👨‍🚀 Wait, UTF8 is variable-width encoded? 🔫👩‍🚀 Always has ��

inline size_t jaUTF8UnitLength(uint8_t c)
{
	// High bits of this header byte indicates how many bytes follows

	if ((c >> 7) == 0) // Single byte unit
		return 0;
	else if ((c >> 5) == 0x06) // 0x06 = 0b00000110, Two-bytes unit
		return 1;
	else if ((c >> 4) == 0x0E) // 0x0E = 0b00001110, Three-bytes unit
		return 2;
	else if ((c >> 3) == 0x1E) // 0x1E = 0b00011110, Four-bytes unit
		return 3;

	return 255;
}

int jaUTF8ValidateUnit(const uint8_t* c, size_t bytes, size_t* out_bytes, uint32_t* out_code)
{
	size_t tail_len = jaUTF8UnitLength(*c);
	uint32_t code = (uint32_t)(*c);

	if (tail_len == 0) // Single byte unit
	{
		if (out_code != NULL)
			*out_code = code;
		if (out_bytes != NULL)
			*out_bytes = 0;

		return 0;
	}
	else if (tail_len > 3 || tail_len > bytes) // Unicode don't encode beyond four bytes
		return 1;

	// Validate tail concatenating code (the encoded thing)
	code = (uint32_t)(*c & (0xFF >> (tail_len + 2)));
	c += 1;

	for (const uint8_t* end = (c + tail_len); c < end; c++)
	{
		// Tail bytes include an header of two bits
		if ((*c >> 6) == 0x02)                          // 0x02 = 0b00000010
			code = (code << 6) | (uint32_t)(*c & 0x3F); // 0x3F = 0b00111111
		else
			return 1;
	}

	// Overloading check
	{
		// Is possible use a longer unit to encode a value that only requires one, two or no tail byte(s).
		// For example, Wikipedia shows that filling the head with zero is possible encode a Euro sign in
		// four bytes rather than three. Same with NULL in the broken Java implementation (Wikipedia).

		// Thing is that above shift-left operation don't care about empty bytes, it just concatenates
		// up to three tail bytes. So we need to check the legal ranges that different tails can encode:

		if ((tail_len == 1 && code < 0x0080)      // One tail byte = U+0080 to U+07FF
		    || (tail_len == 2 && code < 0x0800)   // Two tails bytes = U+0800 to U+FFFF
		    || (tail_len == 3 && code < 0x10000)) // Three tails bytes = U+10000 to U+10FFFF
			return 1;
	}

	if (code > 0x10FFFF) // Last Unicode code point?
		return 1;

	// Bye!
	if (out_code != NULL)
		*out_code = code;
	if (out_bytes != NULL)
		*out_bytes = tail_len;

	return 0;
}

static inline int sUTF8ValidateUnitSimple(const uint8_t* c, size_t* tail_len)
{
	*tail_len = jaUTF8UnitLength(*c);

	if (*tail_len == 0) // Single byte unit
		return 0;
	else if (*tail_len > 3) // Unicode don't encode beyond four bytes
		return 1;

	// Validate tail concatenating code
	uint32_t code = (uint32_t)(*c & (0xFF >> (*tail_len + 2)));
	c += 1;

	for (const uint8_t* end = (c + *tail_len); c < end; c++)
	{
		if ((*c >> 6) == 0x02)                          // 0x02 = 0b00000010
			code = (code << 6) | (uint32_t)(*c & 0x3F); // 0x3F = 0b00111111
		else
			return 1;
	}

	// Overloading check
	if ((*tail_len == 1 && code < 0x0080)      // One tail byte = U+0080 to U+07FF
	    || (*tail_len == 2 && code < 0x0800)   // Two tails bytes = U+0800 to U+FFFF
	    || (*tail_len == 3 && code < 0x10000)) // Three tails bytes = U+10000 to U+10FFFF
		return 1;

	// Bye!
	return (int)(code - 0x10FFFF); // Last Unicode code point?
}

int jaUTF8ValidateString(const uint8_t* string, size_t bytes, size_t* out_bytes, size_t* out_units)
{
	size_t units = 0;
	size_t lenght = 0;
	size_t tail_len = 0;

	for (const uint8_t* string_end = (string + bytes); string < string_end; string++)
	{
		if (sUTF8ValidateUnitSimple(string, &tail_len) != 0)
			break;

		units += 1;
		lenght += 1 + tail_len;

		if (*string == JA_UTF8_NULL)
		{
			if (out_bytes != NULL)
				*out_bytes = lenght; // Counts NULL as part of the string
			if (out_units != NULL)
				*out_units = units; // "

			return 0;
		}

		string += tail_len; // Skip tail
	}

	if (out_bytes != NULL)
		*out_bytes = lenght; // Valid bytes so far
	if (out_units != NULL)
		*out_units = units; // "

	return 1;
}

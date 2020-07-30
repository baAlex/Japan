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

 [string.c]
 - Alexander Brandt 2020

https://en.wikipedia.org/wiki/Ascii
https://en.wikipedia.org/wiki/UTF-8
-----------------------------*/

#include "japan-string.h"


inline int jaASCIIValidateUnit(uint8_t byte)
{
	return (byte <= 127) ? 0 : 1;
}


int jaASCIIValidateString(const uint8_t* string, size_t n, size_t* out_bytes)
{
	size_t lenght = 0;

	for (const uint8_t* string_end = (string + n); string < string_end; string++)
	{
		if (jaASCIIValidateUnit(*string) != 0)
			break;

		lenght += 1;

		if (*string == 0x00) // NULL
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


inline size_t jaUTF8UnitLength(uint8_t head_byte)
{
	// High-bits indicates how many tails the unit uses

	if ((head_byte >> 7) == 0) // Single byte unit
		return 1;
	else if ((head_byte >> 5) == 0x06) // 0x06 = 0b00000110, Two-bytes unit
		return 2;
	else if ((head_byte >> 4) == 0x0E) // 0x0E = 0b00001110, Three-bytes unit
		return 3;
	else if ((head_byte >> 3) == 0x1E) // 0x1E = 0b00011110, Four-bytes unit
		return 4;

	return 255;
}

int jaUTF8ValidateUnit(const uint8_t* byte, size_t n, size_t* out_unit_len, uint32_t* out_unit_code)
{
	size_t unit_len = jaUTF8UnitLength(*byte);

	if (out_unit_len != NULL)
		*out_unit_len = unit_len;

	// Single byte unit
	// Nothing more to do in an old ASCII character
	if (unit_len == 1)
	{
		if (out_unit_code != NULL)
			*out_unit_code = (uint32_t)(*byte);

		return 0;
	}

	// Unicode don't use more than 4 bytes
	else if (unit_len > 4)
		return 1;

	// Our lovely user provided us a truncated unit
	else if (unit_len > n)
		return 1;

	// Validate whole unit
	{
		uint32_t code = (uint32_t)(*byte & (0xFF >> (unit_len + 1))); // Cap head byte
		byte += 1;

		// Concatenate tail bytes
		switch (unit_len - 1)
		{
		case 3:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
			byte += 1;
		case 2:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
			byte += 1;
		case 1:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
		}

		// Overloading check
		// Is possible to encode in a longer unit that the needed
		if ((code < 0x0080 && unit_len == 2)      // Two bytes unit = U+0080 to U+07FF
		    || (code < 0x0800 && unit_len == 3)   // Three bytes unit = U+0800 to U+FFFF
		    || (code < 0x10000 && unit_len == 4)) // Four bytes unit = U+10000 to U+10FFFF
			return 1;

		// UTF-16 invalid codes (surrogates)
		// To keep compatibility allowing conversions
		else if (code >= 0xD800 && code <= 0xDFFF)
			return 1;

		// Last Unicode code
		else if (code > 0x10FFFF) // Planes 15–16, F0000–​10FFFF: 'Supplementary Private Use Area planes'
			return 1;

		// Bye!
		if (out_unit_code != NULL)
			*out_unit_code = code;
	}

	return 0;
}

static inline int sUTF8ValidateUnitSimple(const uint8_t* byte, const uint8_t* end, size_t* unit_len)
{
	*unit_len = jaUTF8UnitLength(*byte);

	if (*unit_len == 1)
		return 0;
	else if (*unit_len > 4 || (byte + *unit_len) > end)
		return 1;

	// Validate whole unit
	{
		uint32_t code = (uint32_t)(*byte & (0xFF >> (*unit_len + 1)));
		byte += 1;

		switch (*unit_len - 1)
		{
		case 3:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
			byte += 1;
		case 2:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
			byte += 1;
		case 1:
			if ((*byte >> 6) == 0x02)                          // 0x02 = 0b00000010
				code = (code << 6) | (uint32_t)(*byte & 0x3F); // 0x3F = 0b00111111
			else
				return 1;
		}

		if ((code < 0x0080 && *unit_len == 2)     // Two bytes unit = U+0080 to U+07FF
		    || (code < 0x0800 && *unit_len == 3)  // Three bytes unit = U+0800 to U+FFFF
		    || (code < 0x10000 && *unit_len == 4) // Four bytes unit = U+10000 to U+10FFFF
		    || (code >= 0xD800 && code <= 0xDFFF) // UTF-16 invalid codes
		    || (code > 0x10FFFF))                 // Last Unicode code
			return 1;
	}

	return 0;
}

int jaUTF8ValidateString(const uint8_t* string, size_t n, size_t* out_bytes, size_t* out_units)
{
	size_t bytes = 0;
	size_t units = 0;

	size_t unit_lenght = 0;

	for (const uint8_t* string_end = (string + n); string < string_end; string++)
	{
		if (sUTF8ValidateUnitSimple(string, string_end, &unit_lenght) != 0)
			break;

		bytes += unit_lenght;
		units += 1;

		if (*string == 0x00) // NULL
		{
			if (out_bytes != NULL)
				*out_bytes = bytes; // Counts NULL as part of the string
			if (out_units != NULL)
				*out_units = units;

			return 0;
		}

		string += unit_lenght - 1;
	}

	if (out_bytes != NULL)
		*out_bytes = bytes; // Valid bytes so far
	if (out_units != NULL)
		*out_units = units;

	return 1;
}

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

 [format-au.c]
 - Alexander Brandt 2019

 http://pubs.opengroup.org/external/auformat.html
-----------------------------*/

#include "../common.h"
#include "private.h"


#define AU_MAGIC 0x2E736E64
#define AU_UNKNOWN_SIZE (~0)

#define AU_ULAW 1
#define AU_PCM8 2
#define AU_PCM16 3
#define AU_PCM24 4 // Unsupported
#define AU_PCM32 5
#define AU_FLOAT 6
#define AU_DOUBLE 7
#define AU_ADPCM_G721 23      // Unsupported
#define AU_ADPCM_G722 24      // Unsupported
#define AU_ADPCM_G723_3BIT 25 // Unsupported
#define AU_ADPCM_G723_5BIT 26 // Unsupported
#define AU_ALAW 27

struct AuHead
{
	uint32_t magic;
	uint32_t data_offset;
	uint32_t data_size;
	uint32_t format;
	uint32_t frequency;
	uint32_t channels;
};


/*-----------------------------

 CheckMagicAu()
-----------------------------*/
bool CheckMagicAu(uint32_t value)
{
	if (jaEndianToU32(value, ENDIAN_BIG, ENDIAN_SYSTEM) == AU_MAGIC)
		return true;

	return false;
}


/*-----------------------------

 jaSoundExLoadAu()
-----------------------------*/
int jaSoundExLoadAu(FILE* file, struct jaSoundEx* out, struct jaStatus* st)
{
	struct AuHead head;
	enum jaEndianness sys_endianness = jaEndianSystem();

	jaStatusSet(st, "jaSoundExLoadAu", STATUS_SUCCESS, NULL);

	if (fread(&head, sizeof(struct AuHead), 1, file) != 1)
	{
		jaStatusSet(st, "jaSoundExLoadAu", STATUS_UNEXPECTED_EOF, "head");
		return 1;
	}

	out->frequency = (size_t)jaEndianToU32(head.frequency, ENDIAN_BIG, sys_endianness);
	out->channels = (size_t)jaEndianToU32(head.channels, ENDIAN_BIG, sys_endianness);
	out->uncompressed_size = (size_t)jaEndianToU32(head.data_size, ENDIAN_BIG, sys_endianness);
	out->endianness = ENDIAN_BIG;
	out->data_offset = (size_t)jaEndianToU32(head.data_offset, ENDIAN_BIG, sys_endianness);
	out->unsigned_8bit = false;
	out->unspecified_size = false;

	if (out->uncompressed_size == (size_t)AU_UNKNOWN_SIZE)
	{
		long current;
		long end;

		if ((current = ftell(file)) == -1 || fseek(file, 0, SEEK_END) != 0)
			goto unknown_size;

		if ((end = ftell(file)) == -1 || fseek(file, current, SEEK_SET) != 0)
			goto unknown_size;

		out->uncompressed_size = ((size_t)end - out->data_offset);

	unknown_size:
		out->uncompressed_size = 0;
		out->unspecified_size = true;
	}

	switch (jaEndianToU32(head.format, ENDIAN_BIG, sys_endianness))
	{
	case AU_PCM8:
		out->length = out->uncompressed_size / out->channels;
		out->format = SOUND_I8;
		out->storage = SOUND_UNCOMPRESSED;
		out->minimum_unit_size = sizeof(int8_t);
		break;
	case AU_PCM16:
		out->length = out->uncompressed_size / sizeof(int16_t) / out->channels;
		out->format = SOUND_I16;
		out->storage = SOUND_UNCOMPRESSED;
		out->minimum_unit_size = sizeof(int16_t);
		break;
	case AU_PCM32:
		out->length = out->uncompressed_size / sizeof(int32_t) / out->channels;
		out->format = SOUND_I32;
		out->storage = SOUND_UNCOMPRESSED;
		out->minimum_unit_size = sizeof(int32_t);
		break;
	case AU_FLOAT:
		out->length = out->uncompressed_size / sizeof(float) / out->channels;
		out->format = SOUND_F32;
		out->storage = SOUND_UNCOMPRESSED;
		out->minimum_unit_size = sizeof(float);
		break;
	case AU_DOUBLE:
		out->length = out->uncompressed_size / sizeof(double) / out->channels;
		out->format = SOUND_F64;
		out->storage = SOUND_UNCOMPRESSED;
		out->minimum_unit_size = sizeof(double);
		break;
	case AU_ULAW:
		out->length = out->uncompressed_size / out->channels;
		out->format = SOUND_I16;
		out->storage = SOUND_ULAW;
		out->minimum_unit_size = sizeof(int16_t);
		out->uncompressed_size = out->uncompressed_size * 2;
		break;
	case AU_ALAW:
		out->length = out->uncompressed_size / out->channels;
		out->format = SOUND_I16;
		out->storage = SOUND_ALAW;
		out->minimum_unit_size = sizeof(int16_t);
		out->uncompressed_size = out->uncompressed_size * 2;
		break;

	default: jaStatusSet(st, "jaSoundExLoadAu", STATUS_UNKNOWN_DATA_FORMAT, NULL); return 1;
	}

	return 0;
}


/*-----------------------------

 jaSoundSaveAu()
-----------------------------*/
int jaSoundSaveAu(const struct jaSound* sound, const char* filename, struct jaStatus* st)
{
	struct AuHead head;
	FILE* file = NULL;
	enum jaEndianness sys_endianness = jaEndianSystem();

	jaStatusSet(st, "jaSoundSaveAu", STATUS_SUCCESS, NULL);

	if (sound->channels > UINT32_MAX || sound->frequency > UINT32_MAX || sound->size > UINT32_MAX)
	{
		jaStatusSet(st, "jaSoundSaveAu", STATUS_UNSUPPORTED_FEATURE, "format ('%s')", filename);
		return 1;
	}

	fopen_s(&file, filename, "wb");

	if (file == NULL)
	{
		jaStatusSet(st, "jaSoundSaveAu", STATUS_FS_ERROR, "'%s'", filename);
		return 1;
	}

	// Head
	head.magic = jaEndianToU32(AU_MAGIC, sys_endianness, ENDIAN_BIG);
	head.data_offset = jaEndianToU32((uint32_t)sizeof(struct AuHead), sys_endianness, ENDIAN_BIG);
	head.data_size = jaEndianToU32((uint32_t)sound->size, sys_endianness, ENDIAN_BIG);
	head.frequency = jaEndianToU32((uint32_t)sound->frequency, sys_endianness, ENDIAN_BIG);
	head.channels = jaEndianToU32((uint32_t)sound->channels, sys_endianness, ENDIAN_BIG);

	switch (sound->format)
	{
	case SOUND_I8: head.format = jaEndianToU32(AU_PCM8, sys_endianness, ENDIAN_BIG); break;
	case SOUND_I16: head.format = jaEndianToU32(AU_PCM16, sys_endianness, ENDIAN_BIG); break;
	case SOUND_I32: head.format = jaEndianToU32(AU_PCM32, sys_endianness, ENDIAN_BIG); break;
	case SOUND_F32: head.format = jaEndianToU32(AU_FLOAT, sys_endianness, ENDIAN_BIG); break;
	case SOUND_F64: head.format = jaEndianToU32(AU_DOUBLE, sys_endianness, ENDIAN_BIG); break;
	}

	if (fwrite(&head, sizeof(struct AuHead), 1, file) != 1)
	{
		jaStatusSet(st, "jaSoundSaveAu", STATUS_IO_ERROR, "head ('%s')", filename);
		goto return_failure;
	}

	// Data
	if (WritePcm(file, sound, ENDIAN_BIG) != 0)
	{
		jaStatusSet(st, "jaSoundSaveAu", STATUS_IO_ERROR, "data ('%s')", filename);
		goto return_failure;
	}

	// Bye!
	return 1;

return_failure:
	fclose(file);
	return 0;
}

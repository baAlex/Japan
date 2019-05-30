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

 [sound-au.c]
 - Alexander Brandt 2019

 http://pubs.opengroup.org/external/auformat.html
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "sound-private.h"

#define AU_MAGIC 0x2E736E64

#define AU_ULAW 1
#define AU_PCM8 2
#define AU_PCM16 3
#define AU_PCM24 4 // Unsupported
#define AU_PCM32 5
#define AU_FLOAT 6
#define AU_DOUBLE 7
#define AU_ADPCM_G721 23	  // Unsupported
#define AU_ADPCM_G722 24	  // Unsupported
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
	if (EndianTo_32(value, ENDIAN_BIG, ENDIAN_SYSTEM) == AU_MAGIC)
		return true;

	return false;
}


/*-----------------------------

 SoundLoadAu()
-----------------------------*/
struct Sound* SoundLoadAu(FILE* file, const char* filename, struct Status* st)
{
	struct Sound* sound = NULL;
	struct AuHead head;
	enum Endianness sys_endianness = EndianSystem();
	size_t length = 0;
	enum SoundFormat format = 0;
	int (*read_function)(FILE*, struct Sound*, enum Endianness);

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);

	// Head
	if (fread(&head, sizeof(struct AuHead), 1, file) != 1)
	{
		StatusSet(st, "SoundLoadAu", STATUS_UNEXPECTED_EOF, "head ('%s')", filename);
		goto return_failure;
	}

	head.data_offset = EndianTo_32(head.data_offset, ENDIAN_BIG, sys_endianness);
	head.data_size = EndianTo_32(head.data_size, ENDIAN_BIG, sys_endianness);
	head.format = EndianTo_32(head.format, ENDIAN_BIG, sys_endianness);
	head.frequency = EndianTo_32(head.frequency, ENDIAN_BIG, sys_endianness);
	head.channels = EndianTo_32(head.channels, ENDIAN_BIG, sys_endianness);

	DEBUG_PRINT("(Au) '%s':\n", filename);
	DEBUG_PRINT(" - Data offset: 0x%04X\n", head.data_offset);
	DEBUG_PRINT(" - Data size: %u bytes\n", head.data_size);
	DEBUG_PRINT(" - Format: %u\n", head.format);
	DEBUG_PRINT(" - Frequency: %u hz\n", head.frequency);
	DEBUG_PRINT(" - Channels: %u\n", head.channels);

	switch (head.format)
	{
	case AU_PCM8:
		length = head.data_size / head.channels;
		format = SOUND_I8;
		read_function = ReadPcm;
		break;
	case AU_PCM16:
		length = head.data_size / sizeof(int16_t) / head.channels;
		format = SOUND_I16;
		read_function = ReadPcm;
		break;
	case AU_PCM32:
		length = head.data_size / sizeof(int32_t) / head.channels;
		format = SOUND_I32;
		read_function = ReadPcm;
		break;
	case AU_FLOAT:
		length = head.data_size / sizeof(float) / head.channels;
		format = SOUND_F32;
		read_function = ReadPcm;
		break;
	case AU_DOUBLE:
		length = head.data_size / sizeof(double) / head.channels;
		format = SOUND_F64;
		read_function = ReadPcm;
		break;

	case AU_ULAW:
		length = head.data_size / head.channels;
		format = SOUND_I16;
		read_function = ReadULaw;
		break;
	case AU_ALAW:
		length = head.data_size / head.channels;
		format = SOUND_I16;
		read_function = ReadALaw;
		break;

	default:
		StatusSet(st, "SoundLoadAu", STATUS_UNKNOWN_DATA_FORMAT, "%s", filename);
		goto return_failure;
	}

	// Data
	if (fseek(file, head.data_offset, SEEK_SET) != 0)
	{
		StatusSet(st, "SoundLoadAu", STATUS_UNEXPECTED_EOF, "at data seek ('%s')", filename);
		goto return_failure;
	}

	if ((sound = SoundCreate(format, length, head.channels, head.frequency)) == NULL)
		goto return_failure;

	if (read_function(file, sound, ENDIAN_BIG) != 0)
	{
		StatusSet(st, "SoundLoadAu", STATUS_UNEXPECTED_EOF, "reading data ('%s')", filename);
		goto return_failure;
	}

	// Bye!
	return sound;

return_failure:
	if (sound != NULL)
		SoundDelete(sound);

	return NULL;
}


/*-----------------------------

 SoundSaveAu()
-----------------------------*/
EXPORT struct Status SoundSaveAu(struct Sound* sound, const char* filename)
{
	struct Status st = {.code = STATUS_SUCCESS};
	struct AuHead head;
	FILE* file = NULL;
	enum Endianness sys_endianness = EndianSystem();

	if (sound->channels > UINT32_MAX || sound->frequency > UINT32_MAX || sound->size > UINT32_MAX)
	{
		StatusSet(&st, "SoundSaveAu", STATUS_UNSUPPORTED_FEATURE, "format ('%s')", filename);
		return st;
	}

	if ((file = fopen(filename, "wb")) == NULL)
	{
		StatusSet(&st, "SoundSaveAu", STATUS_FS_ERROR, "'%s'", filename);
		return st;
	}

	// Head
	head.magic = EndianTo_32(AU_MAGIC, sys_endianness, ENDIAN_BIG);
	head.data_offset = EndianTo_32(sizeof(struct AuHead), sys_endianness, ENDIAN_BIG);
	head.data_size = EndianTo_32((uint32_t)sound->size, sys_endianness, ENDIAN_BIG);
	head.frequency = EndianTo_32((uint32_t)sound->frequency, sys_endianness, ENDIAN_BIG);
	head.channels = EndianTo_32((uint32_t)sound->channels, sys_endianness, ENDIAN_BIG);

	switch (sound->format)
	{
	case SOUND_I8:
		head.format = EndianTo_32(AU_PCM8, sys_endianness, ENDIAN_BIG);
		break;
	case SOUND_I16:
		head.format = EndianTo_32(AU_PCM16, sys_endianness, ENDIAN_BIG);
		break;
	case SOUND_I32:
		head.format = EndianTo_32(AU_PCM32, sys_endianness, ENDIAN_BIG);
		break;
	case SOUND_F32:
		head.format = EndianTo_32(AU_FLOAT, sys_endianness, ENDIAN_BIG);
		break;
	case SOUND_F64:
		head.format = EndianTo_32(AU_DOUBLE, sys_endianness, ENDIAN_BIG);
		break;
	}

	if (fwrite(&head, sizeof(struct AuHead), 1, file) != 1)
	{
		StatusSet(&st, "SoundSaveAu", STATUS_IO_ERROR, "head ('%s')", filename);
		goto return_failure;
	}

	// Data
	if (WritePcm(file, sound, ENDIAN_BIG) != 0)
	{
		StatusSet(&st, "SoundSaveAu", STATUS_IO_ERROR, "data ('%s')", filename);
		goto return_failure;
	}

	// Bye!
return_failure:
	fclose(file);
	return st;
}

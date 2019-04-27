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

#include "endianness.h"
#include "sound-local.h"

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#define AU_MAGIC 0x2E736E64

#define AU_ULAW 1 // TODO
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
#define AU_ALAW 27			  // TODO

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

 sReadPcm()
-----------------------------*/
static int sReadPcm(FILE* file, struct Sound* sound)
{
	int8_t* dst = sound->data;

	if (fread(dst, sound->size, 1, file) != 1)
		return 1;

	if (sound->format != SOUND_I8)
	{
		enum Endianness sys_endianness = EndianSystem();
		size_t bytes = sound->size / sound->length / sound->channels;
		int8_t* end = (int8_t*)sound->data + sound->size;

		if (sound->format == SOUND_I16)
		{
			for (dst = sound->data; dst < end; dst += (bytes * sound->channels))
				for (size_t c = 0; c < sound->channels; c++)
					((uint16_t*)dst)[c] = EndianBigToSystem_16(((uint16_t*)dst)[c], sys_endianness);
		}
		else if (sound->format == SOUND_I32 || sound->format == SOUND_F32)
		{
			for (dst = sound->data; dst < end; dst += (bytes * sound->channels))
				for (size_t c = 0; c < sound->channels; c++)
					((uint32_t*)dst)[c] = EndianBigToSystem_32(((uint32_t*)dst)[c], sys_endianness);
		}
		else if (sound->format == SOUND_F64)
		{
			for (dst = sound->data; dst < end; dst += (bytes * sound->channels))
				for (size_t c = 0; c < sound->channels; c++)
					((uint64_t*)dst)[c] = EndianBigToSystem_64(((uint64_t*)dst)[c], sys_endianness);
		}
	}

	return 0;
}


/*-----------------------------

 CheckMagicAu()
-----------------------------*/
bool CheckMagicAu(uint32_t value)
{
	if (EndianBigToSystem_32(value, ENDIAN_UNKNOWN) == AU_MAGIC)
		return true;

	return false;
}


/*-----------------------------

 SoundLoadAu()
-----------------------------*/
struct Sound* SoundLoadAu(FILE* file, const char* filename, struct Error* e)
{
	struct Sound* sound = NULL;
	struct AuHead head;
	enum Endianness sys_endianness = EndianSystem();
	size_t length = 0;
	enum SoundFormat format = 0;
	int (*read_function)(FILE*, struct Sound*);

	ErrorSet(e, NO_ERROR, NULL, NULL);

	// Head
	if (fread(&head, sizeof(struct AuHead), 1, file) != 1)
	{
		ErrorSet(e, ERROR_BROKEN, "SoundLoadAu", "head ('%s')", filename);
		goto return_failure;
	}

	head.magic = EndianBigToSystem_32(head.magic, sys_endianness);
	head.data_offset = EndianBigToSystem_32(head.data_offset, sys_endianness);
	head.data_size = EndianBigToSystem_32(head.data_size, sys_endianness);
	head.format = EndianBigToSystem_32(head.format, sys_endianness);
	head.frequency = EndianBigToSystem_32(head.frequency, sys_endianness);
	head.channels = EndianBigToSystem_32(head.channels, sys_endianness);

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
		read_function = sReadPcm;
		break;
	case AU_PCM16:
		length = head.data_size / sizeof(int16_t) / head.channels;
		format = SOUND_I16;
		read_function = sReadPcm;
		break;
	case AU_PCM32:
		length = head.data_size / sizeof(int32_t) / head.channels;
		format = SOUND_I32;
		read_function = sReadPcm;
		break;
	case AU_FLOAT:
		length = head.data_size / sizeof(float) / head.channels;
		format = SOUND_F32;
		read_function = sReadPcm;
		break;
	case AU_DOUBLE:
		length = head.data_size / sizeof(double) / head.channels;
		format = SOUND_F64;
		read_function = sReadPcm;
		break;
	default:
		ErrorSet(e, ERROR_UNSUPPORTED, "SoundLoadAu", "data format ('%s')", filename);
		goto return_failure;
	}

	// Data
	if (fseek(file, head.data_offset, SEEK_SET) != 0)
	{
		ErrorSet(e, ERROR_BROKEN, "SoundLoadAu", "data seek ('%s')", filename);
		goto return_failure;
	}

	if ((sound = SoundCreate(format, length, head.channels, head.frequency)) == NULL)
		goto return_failure;

	if (read_function(file, sound) != 0)
	{
		ErrorSet(e, ERROR_BROKEN, "SoundLoadAu", "data ('%s')", filename);
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
struct Error SoundSaveAu(struct Sound* sound, const char* filename)
{
	struct Error e = {.code = NO_ERROR};
	struct AuHead head;
	FILE* file = NULL;
	enum Endianness sys_endianness = EndianSystem();

	if (sound->channels > UINT32_MAX || sound->frequency > UINT32_MAX || sound->size > UINT32_MAX)
	{
		ErrorSet(&e, ERROR_UNSUPPORTED, "SoundSaveAu", "Au format ('%s')", filename);
		return e;
	}

	if ((file = fopen(filename, "wb")) == NULL)
	{
		ErrorSet(&e, ERROR_FS, "SoundSaveAu", "'%s'", filename);
		return e;
	}

	// Head
	head.magic = EndianSystemToBig_32(AU_MAGIC, sys_endianness);
	head.data_offset = EndianSystemToBig_32(sizeof(struct AuHead), sys_endianness);
	head.data_size = EndianSystemToBig_32((uint32_t)sound->size, sys_endianness);
	head.frequency = EndianSystemToBig_32((uint32_t)sound->frequency, sys_endianness);
	head.channels = EndianSystemToBig_32((uint32_t)sound->channels, sys_endianness);

	switch (sound->format)
	{
	case SOUND_I8:
		head.format = EndianSystemToBig_32(AU_PCM8, sys_endianness);
		break;
	case SOUND_I16:
		head.format = EndianSystemToBig_32(AU_PCM16, sys_endianness);
		break;
	case SOUND_I32:
		head.format = EndianSystemToBig_32(AU_PCM32, sys_endianness);
		break;
	case SOUND_F32:
		head.format = EndianSystemToBig_32(AU_FLOAT, sys_endianness);
		break;
	case SOUND_F64:
		head.format = EndianSystemToBig_32(AU_DOUBLE, sys_endianness);
		break;
	}

	if (fwrite(&head, sizeof(struct AuHead), 1, file) != 1)
	{
		ErrorSet(&e, ERROR_IO, "SoundSaveAu", "head ('%s')", filename);
		goto return_failure;
	}

	// Data
	size_t bytes = sound->size / sound->length / sound->channels;
	int8_t* src = NULL;
	int8_t* end = (int8_t*)sound->data + sound->size;

	union {
		uint64_t u64;
		uint32_t u32;
		uint16_t u16;
		uint8_t u8;
	} sample;

	for (src = sound->data; src < end; src += (bytes * sound->channels))
	{
		for (size_t c = 0; c < sound->channels; c++)
		{
			if (sound->format == SOUND_I8)
				sample.u8 = ((uint8_t*)src)[c];
			else if (sound->format == SOUND_I16)
				sample.u16 = EndianBigToSystem_16(((uint16_t*)src)[c], sys_endianness);
			else if (sound->format == SOUND_I32 || sound->format == SOUND_F32)
				sample.u32 = EndianBigToSystem_32(((uint32_t*)src)[c], sys_endianness);
			else if (sound->format == SOUND_F64)
				sample.u64 = EndianBigToSystem_64(((uint64_t*)src)[c], sys_endianness);

			if (fwrite(&sample, bytes, 1, file) != 1)
			{
				ErrorSet(&e, ERROR_IO, "SoundSaveAu", "data ('%s')", filename);
				goto return_failure;
			}
		}
	}

	// Bye!
	fclose(file);
	return e;

return_failure:
	fclose(file);
	return e;
}

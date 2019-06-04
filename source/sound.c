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

SpanDSP - a series of DSP components for telephony
g711.h - In line A-law and u-law conversion routines

Written by Steve Underwood <steveu@coppice.org>

Copyright (C) 2001 Steve Underwood

Despite my general liking of the GPL, I place this code in the
public domain for the benefit of all mankind - even the slimy
ones who might try to proprietize my work and use it to my
detriment.

-------------------------------

 [sound.c]
 - Alexander Brandt 2019

 https://webrtc.googlesource.com/src/+/refs/heads/master/modules/third_party/g711/g711.h
-----------------------------*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "endianness.h"
#include "sound-private.h"

#define ULAW_BIAS 0x84
#define ALAW_AMI_MASK 0x55


static inline int sBytesPerSample(enum SoundFormat format)
{
	switch (format)
	{
	case SOUND_I8:
		return 1;
	case SOUND_I16:
		return 2;
	case SOUND_I32:
	case SOUND_F32:
		return 4;
	case SOUND_F64:
		return 8;
	}

	return 0;
}


/*-----------------------------

 WritePcm()
-----------------------------*/
int WritePcm(FILE* file, struct Sound* sound, enum Endianness dest_endianness)
{
	enum Endianness sys_endianness = EndianSystem();
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
				sample.u16 = EndianTo_16(((uint16_t*)src)[c], sys_endianness, dest_endianness);
			else if (sound->format == SOUND_I32 || sound->format == SOUND_F32)
				sample.u32 = EndianTo_32(((uint32_t*)src)[c], sys_endianness, dest_endianness);
			else if (sound->format == SOUND_F64)
				sample.u64 = EndianTo_64(((uint64_t*)src)[c], sys_endianness, dest_endianness);

			if (fwrite(&sample, bytes, 1, file) != 1)
				return 1;
		}
	}

	return 0;
}


/*-----------------------------

 SoundCreate()
-----------------------------*/
EXPORT struct Sound* SoundCreate(enum SoundFormat format, size_t length, size_t channels, size_t frequency)
{
	struct Sound* sound = NULL;
	size_t size = 0;

	switch (format)
	{
	case SOUND_I8:
		size = sizeof(int8_t) * length * channels;
		break;
	case SOUND_I16:
		size = sizeof(int16_t) * length * channels;
		break;
	case SOUND_I32:
		size = sizeof(int32_t) * length * channels;
		break;
	case SOUND_F32:
		size = sizeof(float) * length * channels;
		break;
	case SOUND_F64:
		size = sizeof(double) * length * channels;
		break;
	}

	if ((sound = malloc(sizeof(struct Sound) + size)) != NULL)
	{
		sound->frequency = frequency;
		sound->channels = channels;
		sound->length = length;
		sound->size = size;
		sound->format = format;
		sound->data = ((struct Sound*)sound + 1);
	}

	return sound;
}


/*-----------------------------

 SoundDelete()
-----------------------------*/
EXPORT void SoundDelete(struct Sound* sound) { free(sound); }


/*-----------------------------

 SoundLoad()
-----------------------------*/
EXPORT struct Sound* SoundLoad(const char* filename, struct Status* st)
{
	FILE* file = NULL;
	struct SoundEx ex = {0};
	struct Sound* sound = NULL;
	uint32_t magic = 0;

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);

	if ((file = fopen(filename, "rb")) == NULL)
	{
		StatusSet(st, "SoundLoad", STATUS_FS_ERROR, "'%s'", filename);
		return NULL;
	}

	if (fread(&magic, sizeof(uint32_t), 1, file) != 1)
	{
		StatusSet(st, "SoundLoad", STATUS_UNEXPECTED_EOF, "near magic ('%s')", filename);
		goto return_failure;
	}

	fseek(file, 0, SEEK_SET);

	// Header
	if (CheckMagicAu(magic) == true)
	{
		if (SoundExLoadAu(file, &ex, st) != 0)
			goto return_failure;

		DEBUG_PRINT("(Au) '%s':\n", filename);
	}
	else if (CheckMagicWav(magic) == true)
	{
		if (SoundExLoadWav(file, &ex, st) != 0)
			goto return_failure;

		DEBUG_PRINT("(Wav) '%s':\n", filename);
	}
	else
	{
		StatusSet(st, "SoundLoad", STATUS_UNKNOWN_FILE_FORMAT, "'%s'", filename);
		goto return_failure;
	}

	DEBUG_PRINT(" - Data size: %lu bytes\n", ex.size);
	DEBUG_PRINT(" - Format: %u\n", ex.format);
	DEBUG_PRINT(" - Frequency: %lu hz\n", ex.frequency);
	DEBUG_PRINT(" - Channels: %lu\n", ex.channels);
	DEBUG_PRINT(" - Data offset: 0x%zX\n", ex.data_offset);

	// Data
	if (fseek(file, ex.data_offset, SEEK_SET) != 0)
	{
		StatusSet(st, "SoundLoad", STATUS_UNEXPECTED_EOF, "at data seek ('%s')", filename);
		goto return_failure;
	}

	if ((sound = SoundCreate(ex.format, ex.length, ex.channels, ex.frequency)) == NULL)
		goto return_failure;

	if (SoundExRead(file, ex, sound->size, sound->data, st) != 0)
	{
		StatusSet(st, "SoundLoad", STATUS_UNEXPECTED_EOF, "reading data ('%s')", filename);
		goto return_failure;
	}

	// Bye!
	fclose(file);
	return sound;

return_failure:
	fclose(file);

	if (sound != NULL)
		SoundDelete(sound);

	return NULL;
}


/*-----------------------------

 SoundSaveRaw()
-----------------------------*/
EXPORT struct Status SoundSaveRaw(struct Sound* sound, const char* filename)
{
	struct Status st = {.code = STATUS_SUCCESS};
	FILE* file = NULL;

	if ((file = fopen(filename, "wb")) == NULL)
	{
		StatusSet(&st, "SoundSaveRaw", STATUS_FS_ERROR, "'%s'", filename);
		return st;
	}

	if (fwrite(sound->data, sound->size, 1, file) != 1)
	{
		StatusSet(&st, "SoundSaveRaw", STATUS_IO_ERROR, "'%s'", filename);
		fclose(file);
		return st;
	}

	fclose(file);
	return st;
}


/*-----------------------------

 SoundExLoad()
-----------------------------*/
EXPORT int SoundExLoad(FILE* file, struct SoundEx* out, struct Status* st)
{
	uint32_t magic = 0;

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);

	if (fread(&magic, sizeof(uint32_t), 1, file) != 1)
	{
		StatusSet(st, "SoundExLoad", STATUS_UNEXPECTED_EOF, "near magic");
		return 1;
	}

	fseek(file, 0, SEEK_SET);

	if (CheckMagicAu(magic) == true)
		return SoundExLoadAu(file, out, st);
	else if (CheckMagicWav(magic) == true)
		return SoundExLoadWav(file, out, st);

	// Unsuccessfully bye!
	StatusSet(st, "SoundExLoad", STATUS_UNKNOWN_FILE_FORMAT, NULL);
	return 1;
}


/*-----------------------------

 SoundExRead()
-----------------------------*/
EXPORT int SoundExRead(FILE* file, struct SoundEx ex, size_t size_to_read, void* out, struct Status* st)
{
	enum Endianness sys_endianness = EndianSystem();
	size_t bps = sBytesPerSample(ex.format);

	uint8_t compressed = 0;
	int t;
	int seg;

	union {
		void* raw;
		int8_t* i8;
		int16_t* i16;
		int32_t* i32;
		float* f32;
		double* f64;
	} dest;

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);
	dest.raw = out;

	if (ex.compression == SOUND_UNCOMPRESSED)
	{
		if (fread(dest.raw, size_to_read, 1, file) != 1)
		{
			StatusSet(st, "SoundExRead", STATUS_UNEXPECTED_EOF, NULL);
			return 1;
		}

		for (size_t i = 0; i < size_to_read; i += bps)
		{
			if (ex.format == SOUND_I8)
			{
				if (ex.oddities.unsigned_8bit == 0)
					break;

				*dest.i8 = *dest.i8 + 0x80;
				dest.i8++;
			}
			else
			{
				if (ex.endianness == sys_endianness)
					break;

				if (ex.format == SOUND_I16)
				{
					*dest.i16 = EndianTo_16(*dest.i16, ex.endianness, sys_endianness);
					dest.i16++;
				}
				else if (ex.format == SOUND_I32 || SOUND_F32)
				{
					*dest.i32 = EndianTo_32(*dest.i32, ex.endianness, sys_endianness);
					dest.i32++;
				}
				else if (ex.format == SOUND_F64)
				{
					*dest.f64 = EndianTo_64(*dest.f64, ex.endianness, sys_endianness);
					dest.f64++;
				}
			}
		}
	}
	else if (ex.compression == SOUND_ALAW)
	{
		for (size_t i = 0; i < size_to_read; i += sizeof(int16_t))
		{
			if (fread(&compressed, sizeof(int8_t), 1, file) != 1)
				return 1;

			compressed ^= ALAW_AMI_MASK;
			i = ((compressed & 0x0F) << 4);
			seg = (((int)compressed & 0x70) >> 4);

			if (seg)
				i = (i + 0x108) << (seg - 1);
			else
				i += 8;

			*dest.i16 = (int16_t)((compressed & 0x80) ? i : -i);
			dest.i16++;
		}
	}
	else if (ex.compression == SOUND_ULAW)
	{
		for (size_t i = 0; i < size_to_read; i += sizeof(int16_t))
		{
			if (fread(&compressed, sizeof(int8_t), 1, file) != 1)
				return 1;

			compressed = ~compressed;
			t = (((compressed & 0x0F) << 3) + ULAW_BIAS) << (((int)compressed & 0x70) >> 4);

			*dest.i16 = (int16_t)((compressed & 0x80) ? (ULAW_BIAS - t) : (t - ULAW_BIAS));
			dest.i16++;
		}
	}

	return 0;
}

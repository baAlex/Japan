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

 [sound.c]
 - Alexander Brandt 2019
-----------------------------*/

#include <stdint.h>
#include <string.h>

#include "../common.h"
#include "private.h"


/*-----------------------------

 WritePcm()
-----------------------------*/
int WritePcm(FILE* file, struct Sound* sound, enum Endianness dest_endianness)
{
	enum Endianness sys_endianness = EndianSystem();
	size_t bps = (size_t)SoundBps(sound->format);

	union {
		uint64_t u64;
		uint32_t u32;
		uint16_t u16;
		uint8_t u8;
	} sample;

	union {
		void* raw;
		uint64_t* u64;
		uint32_t* u32;
		uint16_t* u16;
		uint8_t* u8;
	} src;

	src.raw = sound->data;

	if (sound->format == SOUND_I8)
	{
		if (fwrite(sound->data, sound->size, 1, file) != 1)
			return 1;
	}
	else
	{
		for (size_t i = 0; i < sound->size; i += bps)
		{
			if (sound->format == SOUND_I16)
			{
				sample.u16 = EndianToU16(*src.u16, sys_endianness, dest_endianness);
				src.u16++;
			}
			else if (sound->format == SOUND_I32 || sound->format == SOUND_F32)
			{
				sample.u32 = EndianToU32(*src.u32, sys_endianness, dest_endianness);
				src.u32++;
			}
			else if (sound->format == SOUND_F64)
			{
				sample.u64 = EndianToU64(*src.u64, sys_endianness, dest_endianness);
				src.u64++;
			}

			if (fwrite(&sample, bps, 1, file) != 1)
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
	size_t size = (size_t)SoundBps(format) * length * channels;

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
EXPORT inline void SoundDelete(struct Sound* sound)
{
	free(sound);
}


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

	DEBUG_PRINT(" - Frequency: %zu hz\n", ex.frequency);
	DEBUG_PRINT(" - Channels: %zu\n", ex.channels);
	DEBUG_PRINT(" - Frames: %zu\n", ex.length);
	DEBUG_PRINT(" - Uncompressed size: %zu bytes\n", ex.uncompressed_size);
	DEBUG_PRINT(" - Minimum unit size: %zu bytes\n", ex.minimum_unit_size);
	DEBUG_PRINT(" - Endianness: %s\n", (ex.endianness == ENDIAN_LITTLE) ? "little" : "big");
	DEBUG_PRINT(" - Compression: %i\n", ex.compression);
	DEBUG_PRINT(" - Format: %i\n", ex.format);
	DEBUG_PRINT(" - Data offset: 0x%zX\n", ex.data_offset);

	// Data
	if (fseek(file, (long)ex.data_offset, SEEK_SET) != 0)
	{
		StatusSet(st, "SoundLoad", STATUS_UNEXPECTED_EOF, "at data seek ('%s')", filename);
		goto return_failure;
	}

	if ((sound = SoundCreate(ex.format, ex.length, ex.channels, ex.frequency)) == NULL)
		goto return_failure;

	if (SoundExRead(file, ex, sound->size, sound->data, st) != sound->size)
		goto return_failure;

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
EXPORT size_t SoundExRead(FILE* file, struct SoundEx ex, size_t size_to_read, void* out, struct Status* st)
{
	enum Endianness sys_endianness = EndianSystem();
	size_t bps = (size_t)SoundBps(ex.format);
	size_t bytes_write = 0;

	union {
		void* raw;
		int8_t* i8;
		int16_t* i16;
		int32_t* i32;
		uint64_t* u64;
	} dest;

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);
	dest.raw = out;

	if ((size_to_read % ex.minimum_unit_size) != 0)
	{
		StatusSet(st, "SoundExRead", STATUS_INVALID_ARGUMENT, NULL);
		size_to_read -= (size_to_read % ex.minimum_unit_size);
	}

	if (ex.compression == SOUND_UNCOMPRESSED)
	{
		if (fread(dest.raw, size_to_read, 1, file) != 1)
		{
			StatusSet(st, "SoundExRead", STATUS_UNEXPECTED_EOF, NULL);
			goto return_failure;
		}

		bytes_write = size_to_read;

		for (size_t i = 0; i < size_to_read; i += bps)
		{
			if (ex.format == SOUND_I8)
			{
				if (ex.unsigned_8bit == false)
					break;

				#pragma GCC diagnostic push
				#pragma GCC diagnostic ignored "-Wconversion"
				*dest.i8 = *dest.i8 + 0x80;
				#pragma GCC diagnostic pop

				dest.i8++;
			}
			else
			{
				if (ex.endianness == sys_endianness)
					break;

				if (ex.format == SOUND_I16)
				{
					*dest.i16 = EndianToI16(*dest.i16, ex.endianness, sys_endianness);
					dest.i16++;
				}
				else if (ex.format == SOUND_I32 || ex.format == SOUND_F32)
				{
					*dest.i32 = EndianToI32(*dest.i32, ex.endianness, sys_endianness);
					dest.i32++;
				}
				else if (ex.format == SOUND_F64)
				{
					*dest.u64 = EndianToU64(*dest.u64, ex.endianness, sys_endianness);
					dest.u64++;
				}
			}
		}
	}
	else
	{
		uint8_t compressed = 0;

		for (bytes_write = 0; bytes_write < size_to_read; bytes_write += sizeof(int16_t))
		{
			if (fread(&compressed, sizeof(int8_t), 1, file) != 1)
			{
				StatusSet(st, "SoundExRead", STATUS_UNEXPECTED_EOF, NULL);
				goto return_failure;
			}

			if (ex.compression == SOUND_ALAW)
				*dest.i16 = AlawToInt16(compressed);
			else if (ex.compression == SOUND_ULAW)
				*dest.i16 = UlawToInt16(compressed);

			dest.i16++;
		}
	}

	// Bye!
	return bytes_write;

return_failure:
	return bytes_write;
}


/*-----------------------------

 SoundBps()
-----------------------------*/
EXPORT inline int SoundBps(enum SoundFormat format)
{
	switch (format)
	{
	case SOUND_I8: return 1;
	case SOUND_I16: return 2;
	case SOUND_I32:
	case SOUND_F32: return 4;
	case SOUND_F64: return 8;
	}

	return 0;
}

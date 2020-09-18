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
 - Alexander Brandt 2019-2020
-----------------------------*/

#include <stdint.h>
#include <string.h>

#include "../common.h"
#include "private.h"


/*-----------------------------

 WritePcm()
-----------------------------*/
int WritePcm(FILE* file, const struct jaSound* sound, enum jaEndianness dest_endianness)
{
	enum jaEndianness sys_endianness = jaEndianSystem();
	size_t bps = (size_t)jaBytesPerSample(sound->format);

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

	if (sound->format == JA_SOUND_I8)
	{
		if (fwrite(sound->data, sound->size, 1, file) != 1)
			return 1;
	}
	else
	{
		for (size_t i = 0; i < sound->size; i += bps)
		{
			if (sound->format == JA_SOUND_I16)
			{
				sample.u16 = jaEndianToU16(*src.u16, sys_endianness, dest_endianness);
				src.u16++;
			}
			else if (sound->format == JA_SOUND_I32 || sound->format == JA_SOUND_F32)
			{
				sample.u32 = jaEndianToU32(*src.u32, sys_endianness, dest_endianness);
				src.u32++;
			}
			else if (sound->format == JA_SOUND_F64)
			{
				sample.u64 = jaEndianToU64(*src.u64, sys_endianness, dest_endianness);
				src.u64++;
			}

			if (fwrite(&sample, bps, 1, file) != 1)
				return 1;
		}
	}

	return 0;
}


/*-----------------------------

 jaSoundCreate()
-----------------------------*/
struct jaSound* jaSoundCreate(enum jaSoundFormat format, size_t length, size_t channels, size_t frequency)
{
	struct jaSound* sound = NULL;
	size_t size = (size_t)jaBytesPerSample(format) * length * channels;

	if ((sound = malloc(sizeof(struct jaSound) + size)) != NULL)
	{
		sound->frequency = frequency;
		sound->channels = channels;
		sound->length = length;
		sound->size = size;
		sound->format = format;
		sound->data = ((struct jaSound*)sound + 1);
	}

	return sound;
}


/*-----------------------------

 jaSoundDelete()
-----------------------------*/
inline void jaSoundDelete(struct jaSound* sound)
{
	free(sound);
}


/*-----------------------------

 jaSoundLoad()
-----------------------------*/
struct jaSound* jaSoundLoad(const char* filename, struct jaStatus* st)
{
	FILE* file = NULL;
	struct jaSoundEx ex = {0};
	struct jaSound* sound = NULL;
	uint32_t magic = 0;

	jaStatusSet(st, "jaSoundLoad", JA_STATUS_SUCCESS, NULL);

	if ((file = fopen(filename, "rb")) == NULL)
	{
		jaStatusSet(st, "jaSoundLoad", JA_STATUS_FS_ERROR, "'%s'", filename);
		return NULL;
	}

	if (fread(&magic, sizeof(uint32_t), 1, file) != 1)
	{
		jaStatusSet(st, "jaSoundLoad", JA_STATUS_UNEXPECTED_EOF, "near magic ('%s')", filename);
		goto return_failure;
	}

	fseek(file, 0, SEEK_SET);

	// Header
	if (CheckMagicAu(magic) == true)
	{
		if (SoundExLoadAu(file, &ex, st) != 0)
			goto return_failure;

		JA_DEBUG_PRINT("(Au) '%s':\n", filename);
	}
	else if (CheckMagicWav(magic) == true)
	{
		if (SoundExLoadWav(file, &ex, st) != 0)
			goto return_failure;

		JA_DEBUG_PRINT("(Wav) '%s':\n", filename);
	}
	else
	{
		jaStatusSet(st, "jaSoundLoad", JA_STATUS_UNKNOWN_FILE_FORMAT, "'%s'", filename);
		goto return_failure;
	}

	JA_DEBUG_PRINT(" - Frequency: %zu hz\n", ex.frequency);
	JA_DEBUG_PRINT(" - Channels: %zu\n", ex.channels);
	JA_DEBUG_PRINT(" - Frames: %zu\n", ex.length);
	JA_DEBUG_PRINT(" - Uncompressed size: %zu bytes\n", ex.uncompressed_size);
	JA_DEBUG_PRINT(" - Minimum unit size: %zu bytes\n", ex.minimum_unit_size);
	JA_DEBUG_PRINT(" - jaEndianness: %s\n", (ex.endianness == JA_ENDIAN_LITTLE) ? "little" : "big");
	JA_DEBUG_PRINT(" - Storage: %i\n", ex.storage);
	JA_DEBUG_PRINT(" - Format: %i\n", ex.format);
	JA_DEBUG_PRINT(" - Data offset: 0x%zX\n", ex.data_offset);

	// Data
	if (fseek(file, (long)ex.data_offset, SEEK_SET) != 0)
	{
		jaStatusSet(st, "jaSoundLoad", JA_STATUS_UNEXPECTED_EOF, "at data seek ('%s')", filename);
		goto return_failure;
	}

	if ((sound = jaSoundCreate(ex.format, ex.length, ex.channels, ex.frequency)) == NULL)
	{
		jaStatusSet(st, "jaSoundLoad", JA_STATUS_MEMORY_ERROR, NULL);
		goto return_failure;
	}

	if (jaSoundExRead(file, ex, sound->size, sound->data, st) != sound->size)
		goto return_failure;

	// Bye!
	fclose(file);
	return sound;

return_failure:
	fclose(file);

	if (sound != NULL)
		jaSoundDelete(sound);

	return NULL;
}


/*-----------------------------

 jaSoundSaveRaw()
-----------------------------*/
int jaSoundSaveRaw(const struct jaSound* sound, const char* filename, struct jaStatus* st)
{
	FILE* file = NULL;

	jaStatusSet(st, "jaSoundSaveRaw", JA_STATUS_SUCCESS, NULL);

	if ((file = fopen(filename, "wb")) == NULL)
	{
		jaStatusSet(st, "jaSoundSaveRaw", JA_STATUS_FS_ERROR, "'%s'", filename);
		return 1;
	}

	if (fwrite(sound->data, sound->size, 1, file) != 1)
	{
		jaStatusSet(st, "jaSoundSaveRaw", JA_STATUS_IO_ERROR, "'%s'", filename);
		fclose(file);
		return 1;
	}

	fclose(file);
	return 0;
}


/*-----------------------------

 jaSoundExLoad()
-----------------------------*/
int jaSoundExLoad(FILE* file, struct jaSoundEx* out, struct jaStatus* st)
{
	uint32_t magic = 0;

	jaStatusSet(st, "jaSoundExLoad", JA_STATUS_SUCCESS, NULL);

	if (fread(&magic, sizeof(uint32_t), 1, file) != 1)
	{
		jaStatusSet(st, "jaSoundExLoad", JA_STATUS_UNEXPECTED_EOF, "near magic");
		return 1;
	}

	fseek(file, 0, SEEK_SET);

	if (CheckMagicAu(magic) == true)
		return SoundExLoadAu(file, out, st);
	else if (CheckMagicWav(magic) == true)
		return SoundExLoadWav(file, out, st);

	// Unsuccessfully bye!
	jaStatusSet(st, "jaSoundExLoad", JA_STATUS_UNKNOWN_FILE_FORMAT, NULL);
	return 1;
}


/*-----------------------------

 jaSoundExRead()
-----------------------------*/
size_t jaSoundExRead(FILE* file, struct jaSoundEx ex, size_t size_to_read, void* out, struct jaStatus* st)
{
	enum jaEndianness sys_endianness = jaEndianSystem();
	size_t bps = (size_t)jaBytesPerSample(ex.format);
	size_t bytes_write = 0;

	union {
		void* raw;
		int8_t* i8;
		int16_t* i16;
		int32_t* i32;
		uint64_t* u64;
	} dest;

	jaStatusSet(st, "jaSoundExRead", JA_STATUS_SUCCESS, NULL);
	dest.raw = out;

	if ((size_to_read % ex.minimum_unit_size) != 0)
	{
		jaStatusSet(st, "jaSoundExRead", JA_STATUS_INVALID_ARGUMENT, NULL);
		size_to_read -= (size_to_read % ex.minimum_unit_size);
	}

	if (ex.storage == JA_SOUND_UNCOMPRESSED)
	{
		if (fread(dest.raw, size_to_read, 1, file) != 1)
		{
			jaStatusSet(st, "jaSoundExRead", JA_STATUS_UNEXPECTED_EOF, NULL);
			goto return_failure;
		}

		bytes_write = size_to_read;

		for (size_t i = 0; i < size_to_read; i += bps)
		{
			if (ex.format == JA_SOUND_I8)
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

				if (ex.format == JA_SOUND_I16)
				{
					*dest.i16 = jaEndianToI16(*dest.i16, ex.endianness, sys_endianness);
					dest.i16++;
				}
				else if (ex.format == JA_SOUND_I32 || ex.format == JA_SOUND_F32)
				{
					*dest.i32 = jaEndianToI32(*dest.i32, ex.endianness, sys_endianness);
					dest.i32++;
				}
				else if (ex.format == JA_SOUND_F64)
				{
					*dest.u64 = jaEndianToU64(*dest.u64, ex.endianness, sys_endianness);
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
				jaStatusSet(st, "jaSoundExRead", JA_STATUS_UNEXPECTED_EOF, NULL);
				goto return_failure;
			}

			if (ex.storage == JA_SOUND_ALAW)
				*dest.i16 = AlawToInt16(compressed);
			else if (ex.storage == JA_SOUND_ULAW)
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

 jaBytesPerSample()
-----------------------------*/
inline int jaBytesPerSample(enum jaSoundFormat format)
{
	switch (format)
	{
	case JA_SOUND_I8: return 1;
	case JA_SOUND_I16: return 2;
	case JA_SOUND_I32:
	case JA_SOUND_F32: return 4;
	case JA_SOUND_F64: return 8;
	}

	return 0;
}

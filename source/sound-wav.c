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

 [sound-wav.c]
 - Alexander Brandt 2019

 http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "endianness.h"
#include "local.h"
#include "sound-local.h"

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003 // Requires a Fact block, ignored here
#define WAVE_FORMAT_ALAW 0x0006		  // Same as above
#define WAVE_FORMAT_ULAW 0x0007		  // Same as above
#define WAVE_FORMAT_SUBFORMAT 0xFFFE

#define ID_LEN 4
#define RIFF_ID "RIFF" // Antecedes RiffBlock
#define FMT_ID "fmt "  // Antecedes FmtBlock
#define DATA_ID "data" // Antecedes an array of samples

#define WAVE_SIGNATURE "WAVE"

struct GenericHead
{
	char id[4];
	uint32_t size;
};

struct RiffBlock
{
	// If GenericHead::size >= 4 (*)
	char wave_signature[4];
};

struct FmtBlock
{
	// If GenericHead::size = 16:
	uint16_t format; // WAVE_FORMAT_X
	uint16_t channels;
	uint32_t frequency;
	uint32_t avg_bytes_frequency;
	uint16_t data_align_size;
	uint16_t bits_per_sample;

	// If GenericHead::size = 18:
	uint16_t extension_size;

	// If GenericHead::size = 40:
	uint16_t valid_bits_per_sample;
	uint32_t speakers_mask;
	char subformat[16];
};

// (*) Plus RiffBlock size, it counts the size of all following blocks


/*-----------------------------

 CheckMagicWav()
-----------------------------*/
bool CheckMagicWav(uint32_t value)
{
	if (strncmp((char*)&value, RIFF_ID, ID_LEN) == 0)
		return true;

	return false;
}


/*-----------------------------

 SoundLoadWav()
-----------------------------*/
struct Sound* SoundLoadWav(FILE* file, const char* filename, struct Error* e)
{
	struct Sound* sound = NULL;
	enum Endianness sys_endianness = EndianSystem();

	struct FmtBlock fmt = {0};
	bool fmt_read = false;

	int (*read_function)(FILE*, struct Sound*, enum Endianness);

	union {
		struct GenericHead head;
		struct RiffBlock riff;
	} temp;

	ErrorSet(e, NO_ERROR, NULL, NULL);

	// Riff block
	if (fread(&temp.head, sizeof(struct GenericHead), 1, file) != 1)
	{
		ErrorSet(e, ERROR_BROKEN, "SoundLoadWav", "riff head ('%s')", filename);
		goto return_failure;
	}

	temp.head.size = EndianLittleToSystem_32(temp.head.size, sys_endianness);

	if (strncmp(temp.head.id, RIFF_ID, ID_LEN) != 0 || temp.head.size < sizeof(struct RiffBlock)) // (*a)
	{
		ErrorSet(e, ERROR_UNKNOWN_FORMAT, "SoundLoadWav", "riff block ('%s')", filename);
		goto return_failure;
	}

	if (fread(&temp.riff, sizeof(struct RiffBlock), 1, file) != 1)
	{
		ErrorSet(e, ERROR_BROKEN, "SoundLoadWav", "riff block ('%s')", filename);
		goto return_failure;
	}

	if (strncmp(temp.riff.wave_signature, WAVE_SIGNATURE, ID_LEN) != 0)
	{
		ErrorSet(e, ERROR_UNKNOWN_FORMAT, "SoundLoadWav", "wave signature ('%s')", filename);
		goto return_failure;
	}

	DEBUG_PRINT("(Wav) '%s':\n", filename);

	// The other ones
	while (1)
	{
		// Generic head
		if (fread(&temp.head, sizeof(struct GenericHead), 1, file) != 1)
		{
			if (fmt_read == false)
			{
				ErrorSet(e, ERROR_BROKEN, "SoundLoadWav", "generic head ('%s')", filename);
				goto return_failure;
			}
			else
				break; // We have all we need
		}

		temp.head.size = EndianLittleToSystem_32(temp.head.size, sys_endianness);

		// Format block
		if (strncmp(temp.head.id, FMT_ID, ID_LEN) == 0)
		{
			if (temp.head.size != 16 && temp.head.size != 18 && temp.head.size != 40)
			{
				ErrorSet(e, ERROR_UNKNOWN_FORMAT, "SoundLoadWav", "fmt block ('%s')", filename);
				goto return_failure;
			}

			if (fread(&fmt, temp.head.size, 1, file) != 1)
			{
				ErrorSet(e, ERROR_BROKEN, "SoundLoadWav", "fmt block ('%s')", filename);
				goto return_failure;
			}

			fmt.format = EndianLittleToSystem_16(fmt.format, sys_endianness);
			fmt.channels = EndianLittleToSystem_16(fmt.channels, sys_endianness);
			fmt.frequency = EndianLittleToSystem_32(fmt.frequency, sys_endianness);
			fmt.avg_bytes_frequency = EndianLittleToSystem_32(fmt.avg_bytes_frequency, sys_endianness);
			fmt.data_align_size = EndianLittleToSystem_16(fmt.data_align_size, sys_endianness);
			fmt.bits_per_sample = EndianLittleToSystem_16(fmt.bits_per_sample, sys_endianness);

			DEBUG_PRINT(" - Bits per sample: %u\n", fmt.bits_per_sample);
			DEBUG_PRINT(" - Format: 0x%04X\n", fmt.format);
			DEBUG_PRINT(" - Frequency: %u hz\n", fmt.frequency);
			DEBUG_PRINT(" - Channels: %u\n", fmt.channels);
			DEBUG_PRINT(" - Align: %u\n", fmt.data_align_size);

			fmt_read = true;
		}

		// Data block
		else if (strncmp(temp.head.id, DATA_ID, ID_LEN) == 0)
		{
			enum SoundFormat format = 0;

			// Corrupt file
			if (fmt_read != true)
			{
				printf("TODO :(\n");
				exit(EXIT_FAILURE);
			}

			// Format
			if (fmt.format == WAVE_FORMAT_PCM)
			{
				read_function = ReadPcm;

				if (fmt.bits_per_sample == 8)
					format = SOUND_I8; // FIXME, Wav uses unsigned
				else if (fmt.bits_per_sample == 16)
					format = SOUND_I16;
				else if (fmt.bits_per_sample == 32)
					format = SOUND_I32;
				else
				{
					ErrorSet(e, ERROR_UNSUPPORTED, "SoundLoadWav", "pcm format ('%s')", filename);
					goto return_failure;
				}
			}
			else if (fmt.format == WAVE_FORMAT_IEEE_FLOAT)
			{
				read_function = ReadPcm;

				if (fmt.bits_per_sample == 32)
					format = SOUND_F32;
				else if (fmt.bits_per_sample == 64)
					format = SOUND_F64;
				else
				{
					ErrorSet(e, ERROR_UNSUPPORTED, "SoundLoadWav", "float format ('%s')", filename);
					goto return_failure;
				}
			}
			else if (fmt.format == WAVE_FORMAT_ALAW)
			{
				read_function = ReadALaw;
				format = SOUND_I16;
			}
			else if (fmt.format == WAVE_FORMAT_ULAW)
			{
				read_function = ReadULaw;
				format = SOUND_I16;
			}
			else
			{
				ErrorSet(e, ERROR_UNSUPPORTED, "SoundLoadWav", "data format ('%s')", filename);
				goto return_failure;
			}

			// Data
			if ((sound = SoundCreate(format, temp.head.size / fmt.channels / (fmt.bits_per_sample / 8), fmt.channels,
									 fmt.frequency)) == NULL)
				goto return_failure;

			if (read_function(file, sound, ENDIAN_LITTLE) != 0)
			{
				ErrorSet(e, ERROR_BROKEN, "SoundLoadWav", "data block ('%s')", filename);
				goto return_failure;
			}
		}

		// Ignored block
		else
		{
			DEBUG_PRINT("Ignored '%c%c%c%c' block\n", temp.head.id[0], temp.head.id[1], temp.head.id[2],
						temp.head.id[3]);

			if (fseek(file, temp.head.size, SEEK_CUR) != 0)
			{
				ErrorSet(e, ERROR_BROKEN, "SoundLoadWav", "generic seek ('%s')", filename);
				goto return_failure;
			}
		}
	}

	// Bye!
	return sound;

return_failure:
	if (sound != NULL)
		SoundDelete(sound);

	return NULL;
}


/*-----------------------------

 SoundSaveWav()
-----------------------------*/
export struct Error SoundSaveWav(struct Sound* sound, const char* filename)
{
	(void)sound;
	(void)filename;

	struct Error e = {.code = NO_ERROR};
	return e;
}

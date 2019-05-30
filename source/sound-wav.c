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

 Microsoft (2016) WAVEFORMATEX structure:
 https://docs.microsoft.com/en-us/previous-versions//dd757713(v=vs.85)

 Microsoft (2018) WAVEFORMATEXTENSIBLE structure:
 https://docs.microsoft.com/en-us/windows/desktop/api/mmreg/ns-mmreg-__unnamed_struct_0

 Microsoft (2017) Extensible Wave-Format Descriptors:
 https://docs.microsoft.com/en-us/windows-hardware/drivers/audio/extensible-wave-format-descriptors
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "sound-private.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003 // Requires a Fact block, ignored at loading
#define WAVE_FORMAT_ALAW 0x0006		  // "
#define WAVE_FORMAT_ULAW 0x0007		  // "
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

#define ID_LEN 4
#define RIFF_ID "RIFF" // Antecedes RiffBlock
#define FMT_ID "fmt "  // Antecedes FmtBlock
#define FACT_ID "fact" // Antecedes FactBlock
#define DATA_ID "data" // Antecedes an array of samples

#define WAVE_SIGNATURE "WAVE"

struct GenericHead
{
	char id[4];
	uint32_t size;
};

struct RiffBlock
{
	// If GenericHead::size >= 4 (*a)
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

	// If FmtBlock::format = EXTENSIBLE
	// If GenericHead::size = 18:
	uint16_t extension_size; // Size of following members, can be 0

	// If FmtBlock::extension_size >= 22
	// If GenericHead::size >= 40:
	uint16_t valid_bits_per_sample;
	uint32_t speakers_mask;
	char subformat[16];
};

struct FactBlock
{
	uint32_t samples_no;
};

// (*a) Plus RiffBlock size, it counts the size of all following blocks


/*-----------------------------

 sReadU8Pcm()
-----------------------------*/
static int sReadU8Pcm(FILE* file, struct Sound* sound, enum Endianness org_endianness)
{
	(void)org_endianness;

	uint8_t* dest = sound->data;
	uint8_t* end = (uint8_t*)sound->data + sound->size;

	if (fread(dest, sound->size, 1, file) != 1)
		return 1;

	for (dest = sound->data; dest < end; dest++)
		*dest = *dest + 0x80;

	return 0;
}


/*-----------------------------

 WriteU8Pcm()
-----------------------------*/
static int WriteU8Pcm(FILE* file, struct Sound* sound)
{
	uint8_t* org = NULL;
	uint8_t* end = (uint8_t*)sound->data + sound->size;

	int8_t sample = 0;

	for (org = sound->data; org < end; org++)
	{
		sample = *org + 0x80;

		if (fwrite(&sample, 1, 1, file) != 1)
			return 1;
	}

	return 0;
}


/*-----------------------------

 sReadRiffBlock()
-----------------------------*/
static int sReadRiffBlock(size_t block_size, FILE* file, const char* filename, struct Status* st)
{
	struct RiffBlock riff = {0};

	if (block_size < sizeof(struct RiffBlock)) // (*a)
	{
		StatusSet(st, "SoundLoadWav", STATUS_UNKNOWN_DATA_FORMAT, "riff block ('%s')", filename);
		return 1;
	}

	if (fread(&riff, sizeof(struct RiffBlock), 1, file) != 1)
	{
		StatusSet(st, "SoundLoadWav", STATUS_UNEXPECTED_EOF, "at riff block ('%s')", filename);
		return 2;
	}

	if (strncmp(riff.wave_signature, WAVE_SIGNATURE, ID_LEN) != 0)
	{
		StatusSet(st, "SoundLoadWav", STATUS_UNKNOWN_FILE_FORMAT, "invalid wave signature ('%s')", filename);
		return 3;
	}

	DEBUG_PRINT("(Wav) '%s':\n", filename);
	return 0;
}


/*-----------------------------

 sReadFmtBlock()
-----------------------------*/
static int sReadFmtBlock(size_t block_size, FILE* file, const char* filename, struct FmtBlock* out, struct Status* st)
{
	enum Endianness sys_endianness = EndianSystem();

	if (block_size != 16 && block_size != 18 && block_size != 40)
	{
		StatusSet(st, "SoundLoadWav", STATUS_UNKNOWN_DATA_FORMAT, "fmt block ('%s')", filename);
		return 1;
	}

	if (fread(out, block_size, 1, file) != 1)
	{
		StatusSet(st, "SoundLoadWav", STATUS_UNEXPECTED_EOF, "at fmt block ('%s')", filename);
		return 2;
	}

	out->format = EndianTo_16(out->format, ENDIAN_LITTLE, sys_endianness);
	out->channels = EndianTo_16(out->channels, ENDIAN_LITTLE, sys_endianness);
	out->frequency = EndianTo_32(out->frequency, ENDIAN_LITTLE, sys_endianness);
	out->avg_bytes_frequency = EndianTo_32(out->avg_bytes_frequency, ENDIAN_LITTLE, sys_endianness);
	out->data_align_size = EndianTo_16(out->data_align_size, ENDIAN_LITTLE, sys_endianness);
	out->bits_per_sample = EndianTo_16(out->bits_per_sample, ENDIAN_LITTLE, sys_endianness);

	DEBUG_PRINT(" - Block size: %lu\n", block_size);
	DEBUG_PRINT(" - Bits per sample: %u\n", out->bits_per_sample);
	DEBUG_PRINT(" - Format: 0x%04X\n", out->format);
	DEBUG_PRINT(" - Frequency: %u hz\n", out->frequency);
	DEBUG_PRINT(" - Channels: %u\n", out->channels);
	DEBUG_PRINT(" - Align: %u\n", out->data_align_size);

	return 0;
}


/*-----------------------------

 sReadDataBlock()
-----------------------------*/
static struct Sound* sReadDataBlock(size_t block_size, FILE* file, const char* filename, struct FmtBlock* fmt,
									struct Status* st)
{
	struct Sound* sound = NULL;

	enum SoundFormat format;
	int (*read_function)(FILE*, struct Sound*, enum Endianness);

	if (fmt->format == WAVE_FORMAT_ALAW)
	{
		read_function = ReadALaw;
		format = SOUND_I16;
	}
	else if (fmt->format == WAVE_FORMAT_ULAW)
	{
		read_function = ReadULaw;
		format = SOUND_I16;
	}
	else if (fmt->format == WAVE_FORMAT_IEEE_FLOAT)
	{
		read_function = ReadPcm;

		if (fmt->bits_per_sample == 32)
			format = SOUND_F32;
		else if (fmt->bits_per_sample == 64)
			format = SOUND_F64;
		else
		{
			StatusSet(st, "SoundLoadWav", STATUS_UNSUPPORTED_FEATURE, "float format (%u, '%s')", fmt->bits_per_sample, filename);
			return NULL;
		}
	}
	else if (fmt->format == WAVE_FORMAT_PCM)
	{
		if (fmt->bits_per_sample == 8)
		{
			format = SOUND_I8;
			read_function = sReadU8Pcm;
		}
		else if (fmt->bits_per_sample == 16)
		{
			format = SOUND_I16;
			read_function = ReadPcm;
		}
		else if (fmt->bits_per_sample == 32)
		{
			// Audacity ignores Microsoft (2016) advice?:
			// "If wFormatTag is WAVE_FORMAT_PCM, then wBitsPerSample should be equal to 8 or 16."
			format = SOUND_I32;
			read_function = ReadPcm;
		}
		else
		{
			StatusSet(st, "SoundLoadWav", STATUS_UNSUPPORTED_FEATURE, "pcm format (%u, '%s')", fmt->bits_per_sample, filename);
			return NULL;
		}
	}
	else if (fmt->format == WAVE_FORMAT_EXTENSIBLE)
	{
		if (fmt->extension_size < 22)
		{
			StatusSet(st, "SoundLoadWav", STATUS_UNEXPECTED_EOF, "extensible format section ('%s')", filename);
			return NULL;
		}

		// TODO
		StatusSet(st, "SoundSaveWav", STATUS_UNSUPPORTED_FEATURE, "extensible format ('%s')", filename);
		return NULL;
	}
	else
	{
		StatusSet(st, "SoundLoadWav", STATUS_UNKNOWN_DATA_FORMAT, "'%s'", filename);
		return NULL;
	}

	// Read!
	if ((sound = SoundCreate(format, block_size / fmt->channels / (fmt->bits_per_sample / 8), fmt->channels,
							 fmt->frequency)) == NULL)
		return NULL;

	if (read_function(file, sound, ENDIAN_LITTLE) != 0)
	{
		StatusSet(st, "SoundLoadWav", STATUS_UNEXPECTED_EOF, "at data block ('%s')", filename);
		return NULL;
	}

	return sound;
}


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
struct Sound* SoundLoadWav(FILE* file, const char* filename, struct Status* st)
{
	struct Sound* sound = NULL;

	bool riff_read = false;
	bool fmt_read = false;
	bool data_read = false;

	struct GenericHead head;
	struct FmtBlock fmt = {0};

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);

	while (1)
	{
		// Generic head
		if (fread(&head, sizeof(struct GenericHead), 1, file) != 1)
		{
			if (riff_read == false || fmt_read == false || data_read == false)
			{
				StatusSet(st, "SoundLoadWav", STATUS_UNEXPECTED_EOF, "at generic head ('%s')", filename);
				goto return_failure;
			}
			else
				break; // We have all what we need
		}

		head.size = EndianTo_32(head.size, ENDIAN_LITTLE, ENDIAN_SYSTEM);

		// Riff block
		if (strncmp(head.id, RIFF_ID, ID_LEN) == 0)
		{
			if (sReadRiffBlock(head.size, file, filename, st) != 0)
				goto return_failure;

			riff_read = true;
		}

		// Format block
		else if (strncmp(head.id, FMT_ID, ID_LEN) == 0)
		{
			if (riff_read == false)
			{
				StatusSet(st, "SoundLoadWav", STATUS_UNEXPECTED_DATA, "expected riff block ('%s')", filename);
				goto return_failure;
			}

			if (sReadFmtBlock(head.size, file, filename, &fmt, st) != 0)
				goto return_failure;

			fmt_read = true;
		}

		// Data block
		else if (strncmp(head.id, DATA_ID, ID_LEN) == 0)
		{
			if (fmt_read == false)
			{
				StatusSet(st, "SoundLoadWav", STATUS_UNEXPECTED_DATA, "expected fmt block ('%s')", filename);
				goto return_failure;
			}

			if ((sound = sReadDataBlock(head.size, file, filename, &fmt, st)) == NULL)
				goto return_failure;

			data_read = true;
		}

		// Ignored block
		else
		{
			DEBUG_PRINT("Ignored '%c%c%c%c' block\n", head.id[0], head.id[1], head.id[2], head.id[3]);

			if (fseek(file, head.size, SEEK_CUR) != 0)
			{
				StatusSet(st, "SoundLoadWav", STATUS_UNEXPECTED_EOF, "at generic seek ('%s')", filename);
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
EXPORT struct Status SoundSaveWav(struct Sound* sound, const char* filename)
{
	struct Status st = {.code = STATUS_SUCCESS};
	FILE* file = NULL;
	enum Endianness sys_endianness = EndianSystem();
	struct GenericHead head;

	if ((file = fopen(filename, "wb")) == NULL)
	{
		StatusSet(&st, "SoundSaveWav", STATUS_FS_ERROR, "'%s'", filename);
		return st;
	}

	// Riff block
	{
		struct RiffBlock riff = {0};

		size_t size = sizeof(struct RiffBlock); // Generic head size ignored because blocks on the riff format follow a
												// inheritance (tree like) arrange
		size += sizeof(struct GenericHead) + sizeof(struct FactBlock);
		size += sizeof(struct GenericHead) + 16;		  // FmtBlock
		size += sizeof(struct GenericHead) + sound->size; // DataBlock

		memcpy(head.id, RIFF_ID, ID_LEN);
		head.size = EndianTo_32(size, sys_endianness, ENDIAN_LITTLE); // (*a)

		if (fwrite(&head, sizeof(struct GenericHead), 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "riff head ('%s')", filename);
			goto return_failure;
		}

		memcpy(riff.wave_signature, WAVE_SIGNATURE, ID_LEN);

		if (fwrite(&riff, sizeof(struct RiffBlock), 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "riff block ('%s')", filename);
			goto return_failure;
		}
	}

	// Fmt block (note size = 16 bytes)
	{
		struct FmtBlock fmt = {0};

		memcpy(head.id, FMT_ID, ID_LEN);
		head.size = EndianTo_32(16, sys_endianness, ENDIAN_LITTLE);

		if (fwrite(&head, sizeof(struct GenericHead), 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "fmt head ('%s')", filename);
			goto return_failure;
		}

		switch (sound->format)
		{
		case SOUND_I8:
			fmt.format = EndianTo_16(WAVE_FORMAT_PCM, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo_16(8, sys_endianness, ENDIAN_LITTLE);
			break;

		case SOUND_I16:
			fmt.format = EndianTo_16(WAVE_FORMAT_PCM, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo_16(16, sys_endianness, ENDIAN_LITTLE);
			break;

		case SOUND_I32: // Audacity way (FIXME?)
			fmt.format = EndianTo_16(WAVE_FORMAT_PCM, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo_16(32, sys_endianness, ENDIAN_LITTLE);
			break;

		case SOUND_F32:
			fmt.format = EndianTo_16(WAVE_FORMAT_IEEE_FLOAT, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo_16(32, sys_endianness, ENDIAN_LITTLE);
			break;

		case SOUND_F64:
			fmt.format = EndianTo_16(WAVE_FORMAT_IEEE_FLOAT, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo_16(64, sys_endianness, ENDIAN_LITTLE);
			break;
		}

		fmt.channels = EndianTo_16(sound->channels, sys_endianness, ENDIAN_LITTLE);
		fmt.frequency = EndianTo_32(sound->frequency, sys_endianness, ENDIAN_LITTLE);
		fmt.avg_bytes_frequency = EndianTo_32(0, sys_endianness, ENDIAN_LITTLE); // FIXME
		fmt.data_align_size = EndianTo_16(0, sys_endianness, ENDIAN_LITTLE);	 // FIXME

		if (fwrite(&fmt, 16, 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "fmt block ('%s')", filename);
			goto return_failure;
		}
	}

	// Fact block (while optional, writing it is somewhat better, no?)
	{
		struct FactBlock fact = {0};

		memcpy(head.id, FACT_ID, ID_LEN);
		head.size = EndianTo_32(sizeof(struct FactBlock), sys_endianness, ENDIAN_LITTLE);

		if (fwrite(&head, sizeof(struct GenericHead), 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "fact head ('%s')", filename);
			goto return_failure;
		}

		fact.samples_no = EndianTo_32(sound->length, sys_endianness, ENDIAN_LITTLE);

		if (fwrite(&fact, sizeof(struct FactBlock), 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "fact block ('%s')", filename);
			goto return_failure;
		}
	}

	// Data block
	{
		int ret = 0;

		memcpy(head.id, DATA_ID, ID_LEN);
		head.size = EndianTo_32(sound->size, sys_endianness, ENDIAN_LITTLE);

		if (fwrite(&head, sizeof(struct GenericHead), 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "data head ('%s')", filename);
			goto return_failure;
		}

		ret = (sound->format == SOUND_I8) ? WriteU8Pcm(file, sound) : WritePcm(file, sound, ENDIAN_LITTLE);

		if (ret != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "data block ('%s')", filename);
			goto return_failure;
		}
	}

	// Bye!
return_failure:
	fclose(file);
	return st;
}

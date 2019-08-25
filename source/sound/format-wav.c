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

 [format-wav.c]
 - Alexander Brandt 2019

 http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

 Microsoft (2016) WAVEFORMATEX structure:
 https://docs.microsoft.com/en-us/previous-versions//dd757713(v=vs.85)

 Microsoft (2018) WAVEFORMATEXTENSIBLE structure:
 https://docs.microsoft.com/en-us/windows/desktop/api/mmreg/ns-mmreg-__unnamed_struct_0

 Microsoft (2017) Extensible Wave-Format Descriptors:
 https://docs.microsoft.com/en-us/windows-hardware/drivers/audio/extensible-wave-format-descriptors
-----------------------------*/

#include "private.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003 // Requires a Fact block, ignored at loading
#define WAVE_FORMAT_ALAW 0x0006       // "
#define WAVE_FORMAT_ULAW 0x0007       // "
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

 WriteU8Pcm()
-----------------------------*/
static int WriteU8Pcm(FILE* file, struct Sound* sound)
{
	uint8_t* org = NULL;
	uint8_t* end = (uint8_t*)sound->data + sound->size;

	uint8_t sample = 0;

	for (org = sound->data; org < end; org++)
	{
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wconversion"
		sample = *org + 0x80;
		#pragma GCC diagnostic pop

		if (fwrite(&sample, 1, 1, file) != 1)
			return 1;
	}

	return 0;
}


/*-----------------------------

 sReadRiffBlock()
-----------------------------*/
static int sReadRiffBlock(size_t block_size, FILE* file, struct Status* st)
{
	struct RiffBlock riff = {0};

	if (block_size < sizeof(struct RiffBlock)) // (*a)
	{
		StatusSet(st, "SoundExLoadWav", STATUS_UNKNOWN_DATA_FORMAT, "riff block");
		return 1;
	}

	if (fread(&riff, sizeof(struct RiffBlock), 1, file) != 1)
	{
		StatusSet(st, "SoundExLoadWav", STATUS_UNEXPECTED_EOF, "at riff block");
		return 1;
	}

	if (strncmp(riff.wave_signature, WAVE_SIGNATURE, ID_LEN) != 0)
	{
		StatusSet(st, "SoundExLoadWav", STATUS_UNKNOWN_FILE_FORMAT, "invalid wave signature");
		return 1;
	}

	return 0;
}


/*-----------------------------

 sReadFmtBlock()
-----------------------------*/
static int sReadFmtBlock(size_t block_size, FILE* file, struct SoundEx* out, struct Status* st)
{
	// Audacity ignores Microsoft (2016) advice?:
	// "If wFormatTag is WAVE_FORMAT_PCM, then wBitsPerSample should be equal to 8 or 16."

	enum Endianness sys_endianness = EndianSystem();
	struct FmtBlock fmt;

	if (block_size != 16 && block_size != 18 && block_size != 40)
	{
		StatusSet(st, "SoundExLoadWav", STATUS_UNKNOWN_DATA_FORMAT, "fmt block");
		return 1;
	}

	if (fread(&fmt, block_size, 1, file) != 1)
	{
		StatusSet(st, "SoundExLoadWav", STATUS_UNEXPECTED_EOF, "at fmt block");
		return 1;
	}

	out->frequency = (size_t)EndianTo(fmt.frequency, ENDIAN_LITTLE, sys_endianness);
	out->channels = (size_t)EndianTo(fmt.channels, ENDIAN_LITTLE, sys_endianness);
	out->endianness = ENDIAN_LITTLE;
	out->unsigned_8bit = true;
	out->unspecified_size = false;

	fmt.format = EndianTo(fmt.format, ENDIAN_LITTLE, sys_endianness);
	fmt.bits_per_sample = EndianTo(fmt.bits_per_sample, ENDIAN_LITTLE, sys_endianness);

	if (fmt.format == WAVE_FORMAT_ULAW)
	{
		out->format = SOUND_I16;
		out->compression = SOUND_ULAW;
		out->minimum_unit_size = sizeof(int16_t);
	}
	else if (fmt.format == WAVE_FORMAT_ALAW)
	{
		out->format = SOUND_I16;
		out->compression = SOUND_ALAW;
		out->minimum_unit_size = sizeof(int16_t);
	}
	else if (fmt.format == WAVE_FORMAT_PCM)
	{
		out->compression = SOUND_UNCOMPRESSED;
		out->minimum_unit_size = fmt.bits_per_sample / 8;

		if (fmt.bits_per_sample == 8)
			out->format = SOUND_I8;
		else if (fmt.bits_per_sample == 16)
			out->format = SOUND_I16;
		else if (fmt.bits_per_sample == 32)
			out->format = SOUND_I32;
		else
		{
			StatusSet(st, "SoundExLoadWav", STATUS_UNSUPPORTED_FEATURE, "pcm bits per sample");
			return 1;
		}
	}
	else if (fmt.format == WAVE_FORMAT_IEEE_FLOAT)
	{
		out->compression = SOUND_UNCOMPRESSED;
		out->minimum_unit_size = fmt.bits_per_sample / 8;

		if (fmt.bits_per_sample == 32)
			out->format = SOUND_F32;
		else if (fmt.bits_per_sample == 64)
			out->format = SOUND_F64;
		else
		{
			StatusSet(st, "SoundExLoadWav", STATUS_UNSUPPORTED_FEATURE, "float bits per sample");
			return 1;
		}
	}
	else if (fmt.format == WAVE_FORMAT_EXTENSIBLE)
	{
		StatusSet(st, "SoundExLoadWav", STATUS_UNSUPPORTED_FEATURE, "extensible format");
		return 1;
	}
	else
	{
		StatusSet(st, "SoundExLoadWav", STATUS_UNKNOWN_DATA_FORMAT, NULL);
		return 1;
	}

	return 0;
}


/*-----------------------------

 sReadDataBlock()
-----------------------------*/
static int sReadDataBlock(size_t block_size, FILE* file, struct SoundEx* out, struct Status* st)
{
	out->length = block_size / out->channels / (size_t)SoundBps(out->format);
	out->uncompressed_size = block_size;
	out->data_offset = (size_t)ftell(file);

	if (out->compression != SOUND_UNCOMPRESSED)
		out->uncompressed_size = block_size * 2;

	if (fseek(file, (long)block_size, SEEK_CUR) != 0)
	{
		StatusSet(st, "SoundExLoadWav", STATUS_UNEXPECTED_EOF, "at generic seek");
		return 1;
	}

	return 0;
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

 SoundExLoadWav()
-----------------------------*/
int SoundExLoadWav(FILE* file, struct SoundEx* out, struct Status* st)
{
	bool riff_read = false;
	bool fmt_read = false;
	bool data_read = false; // Not the actual data, but the data block

	struct GenericHead head;

	StatusSet(st, NULL, STATUS_SUCCESS, NULL);

	while (1)
	{
		// Generic head
		if (fread(&head, sizeof(struct GenericHead), 1, file) != 1)
		{
			if (riff_read == false || fmt_read == false || data_read == false)
			{
				StatusSet(st, "SoundExLoadWav", STATUS_UNEXPECTED_EOF, "at generic head");
				return 1;
			}
			else
				break; // We have all what we need
		}

		head.size = EndianTo(head.size, ENDIAN_LITTLE, ENDIAN_SYSTEM);

		// Riff block
		if (strncmp(head.id, RIFF_ID, ID_LEN) == 0)
		{
			if (sReadRiffBlock(head.size, file, st) != 0)
				return 1;

			riff_read = true;
		}

		// Format block
		else if (strncmp(head.id, FMT_ID, ID_LEN) == 0)
		{
			if (riff_read == false)
			{
				StatusSet(st, "SoundExLoadWav", STATUS_UNEXPECTED_DATA, "expected riff block");
				return 1;
			}

			if (sReadFmtBlock(head.size, file, out, st) != 0)
				return 1;

			fmt_read = true;
		}

		// Data block
		else if (strncmp(head.id, DATA_ID, ID_LEN) == 0)
		{
			if (fmt_read == false)
			{
				StatusSet(st, "SoundExLoadWav", STATUS_UNEXPECTED_DATA, "expected fmt block");
				return 1;
			}

			if (sReadDataBlock(head.size, file, out, st) != 1)
				data_read = true;
		}

		// Ignored block
		else
		{
			DEBUG_PRINT("Ignored '%c%c%c%c' block\n", head.id[0], head.id[1], head.id[2], head.id[3]);

			if (fseek(file, head.size, SEEK_CUR) != 0)
			{
				StatusSet(st, "SoundExLoadWav", STATUS_UNEXPECTED_EOF, "at generic seek");
				return 1;
			}
		}
	}

	return 0;
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
	size_t bps = (size_t)SoundBps(sound->format);

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
		size += sizeof(struct GenericHead) + 16;          // FmtBlock
		size += sizeof(struct GenericHead) + sound->size; // DataBlock

		memcpy(head.id, RIFF_ID, ID_LEN);
		head.size = EndianTo((uint32_t)size, sys_endianness, ENDIAN_LITTLE); // (*a)

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
		head.size = EndianTo((uint32_t)16, sys_endianness, ENDIAN_LITTLE);

		if (fwrite(&head, sizeof(struct GenericHead), 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "fmt head ('%s')", filename);
			goto return_failure;
		}

		switch (sound->format)
		{
		case SOUND_I8:
			fmt.format = EndianTo((uint16_t)WAVE_FORMAT_PCM, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo((uint16_t)8, sys_endianness, ENDIAN_LITTLE);
			break;

		case SOUND_I16:
			fmt.format = EndianTo((uint16_t)WAVE_FORMAT_PCM, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo((uint16_t)16, sys_endianness, ENDIAN_LITTLE);
			break;

		case SOUND_I32: // Audacity way
			fmt.format = EndianTo((uint16_t)WAVE_FORMAT_PCM, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo((uint16_t)32, sys_endianness, ENDIAN_LITTLE);
			break;

		case SOUND_F32:
			fmt.format = EndianTo((uint16_t)WAVE_FORMAT_IEEE_FLOAT, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo((uint16_t)32, sys_endianness, ENDIAN_LITTLE);
			break;

		case SOUND_F64:
			fmt.format = EndianTo((uint16_t)WAVE_FORMAT_IEEE_FLOAT, sys_endianness, ENDIAN_LITTLE);
			fmt.bits_per_sample = EndianTo((uint16_t)64, sys_endianness, ENDIAN_LITTLE);
			break;
		}

		fmt.channels = EndianTo((uint16_t)sound->channels, sys_endianness, ENDIAN_LITTLE);
		fmt.frequency = EndianTo((uint32_t)sound->frequency, sys_endianness, ENDIAN_LITTLE);
		fmt.avg_bytes_frequency = EndianTo((uint32_t)(sound->channels * sound->frequency * bps), sys_endianness, ENDIAN_LITTLE);
		fmt.data_align_size = EndianTo((uint16_t)(sound->channels * bps), sys_endianness, ENDIAN_LITTLE);

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
		head.size = EndianTo((uint32_t)sizeof(struct FactBlock), sys_endianness, ENDIAN_LITTLE);

		if (fwrite(&head, sizeof(struct GenericHead), 1, file) != 1)
		{
			StatusSet(&st, "SoundSaveWav", STATUS_IO_ERROR, "fact head ('%s')", filename);
			goto return_failure;
		}

		fact.samples_no = EndianTo((uint32_t)sound->length, sys_endianness, ENDIAN_LITTLE);

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
		head.size = EndianTo((uint32_t)sound->size, sys_endianness, ENDIAN_LITTLE);

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

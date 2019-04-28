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
 https://github.com/fffaraz/Multimedia/tree/master/G.711 // I am lazy
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


static int16_t s_ulaw[256] = {
	-32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956, -23932, -22908, -21884, -20860, -19836, -18812,
	-17788, -16764, -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412, -11900, -11388, -10876, -10364,
	-9852,  -9340,  -8828,  -8316,  -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,  -5884,  -5628,
	-5372,  -5116,  -4860,  -4604,  -4348,  -4092,  -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
	-2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,  -1884,  -1820,  -1756,  -1692,  -1628,  -1564,
	-1500,  -1436,  -1372,  -1308,  -1244,  -1180,  -1116,  -1052,  -988,   -924,   -876,   -844,   -812,   -780,
	-748,   -716,   -684,   -652,   -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,   -372,   -356,
	-340,   -324,   -308,   -292,   -276,   -260,   -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
	-120,   -112,   -104,   -96,	-88,	-80,	-72,	-64,	-56,	-48,	-40,	-32,	-24,	-16,
	-8,		-1,		32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,  23932,  22908,  21884,  20860,
	19836,  18812,  17788,  16764,  15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,  11900,  11388,
	10876,  10364,  9852,   9340,   8828,   8316,   7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
	5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,   3900,   3772,   3644,   3516,   3388,   3260,
	3132,   3004,   2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,   1884,   1820,   1756,   1692,
	1628,   1564,   1500,   1436,   1372,   1308,   1244,   1180,   1116,   1052,   988,	924,	876,	844,
	812,	780,	748,	716,	684,	652,	620,	588,	556,	524,	492,	460,	428,	396,
	372,	356,	340,	324,	308,	292,	276,	260,	244,	228,	212,	196,	180,	164,
	148,	132,	120,	112,	104,	96,		88,		80,		72,		64,		56,		48,		40,		32,
	24,		16,		8,		0};

static int16_t s_alaw[256] = {
	-5504,  -5248,  -6016,  -5760,  -4480,  -4224,  -4992,  -4736,  -7552,  -7296,  -8064,  -7808,  -6528,  -6272,
	-7040,  -6784,  -2752,  -2624,  -3008,  -2880,  -2240,  -2112,  -2496,  -2368,  -3776,  -3648,  -4032,  -3904,
	-3264,  -3136,  -3520,  -3392,  -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944, -30208, -29184,
	-32256, -31232, -26112, -25088, -28160, -27136, -11008, -10496, -12032, -11520, -8960,  -8448,  -9984,  -9472,
	-15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568, -344,   -328,   -376,   -360,   -280,   -264,
	-312,   -296,   -472,   -456,   -504,   -488,   -408,   -392,   -440,   -424,   -88,	-72,	-120,   -104,
	-24,	-8,		-56,	-40,	-216,   -200,   -248,   -232,   -152,   -136,   -184,   -168,   -1376,  -1312,
	-1504,  -1440,  -1120,  -1056,  -1248,  -1184,  -1888,  -1824,  -2016,  -1952,  -1632,  -1568,  -1760,  -1696,
	-688,   -656,   -752,   -720,   -560,   -528,   -624,   -592,   -944,   -912,   -1008,  -976,   -816,   -784,
	-880,   -848,   5504,   5248,   6016,   5760,   4480,   4224,   4992,   4736,   7552,   7296,   8064,   7808,
	6528,   6272,   7040,   6784,   2752,   2624,   3008,   2880,   2240,   2112,   2496,   2368,   3776,   3648,
	4032,   3904,   3264,   3136,   3520,   3392,   22016,  20992,  24064,  23040,  17920,  16896,  19968,  18944,
	30208,  29184,  32256,  31232,  26112,  25088,  28160,  27136,  11008,  10496,  12032,  11520,  8960,   8448,
	9984,   9472,   15104,  14592,  16128,  15616,  13056,  12544,  14080,  13568,  344,	328,	376,	360,
	280,	264,	312,	296,	472,	456,	504,	488,	408,	392,	440,	424,	88,		72,
	120,	104,	24,		8,		56,		40,		216,	200,	248,	232,	152,	136,	184,	168,
	1376,   1312,   1504,   1440,   1120,   1056,   1248,   1184,   1888,   1824,   2016,   1952,   1632,   1568,
	1760,   1696,   688,	656,	752,	720,	560,	528,	624,	592,	944,	912,	1008,   976,
	816,	784,	880,	848};


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

 sReadULaw()
-----------------------------*/
static int sReadULaw(FILE* file, struct Sound* sound)
{
	int16_t* end = (int16_t*)sound->data + (sound->length * sound->channels);
	uint8_t compressed_frame = 0;

	for (int16_t* dst = sound->data; dst < end; dst++)
	{
		if (fread(&compressed_frame, sizeof(uint8_t), 1, file) != 1)
			return 1;

		*dst = s_ulaw[compressed_frame];
	}

	return 0;
}


/*-----------------------------

 sReadALaw()
-----------------------------*/
static int sReadALaw(FILE* file, struct Sound* sound)
{
	int16_t* end = (int16_t*)sound->data + (sound->length * sound->channels);
	uint8_t compressed_frame = 0;

	for (int16_t* dst = sound->data; dst < end; dst++)
	{
		if (fread(&compressed_frame, sizeof(uint8_t), 1, file) != 1)
			return 1;

		*dst = s_alaw[compressed_frame];
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

	case AU_ULAW:
		length = head.data_size / head.channels;
		format = SOUND_I16;
		read_function = sReadULaw;
		break;
	case AU_ALAW:
		length = head.data_size / head.channels;
		format = SOUND_I16;
		read_function = sReadALaw;
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

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

 $ clang "source/sound.c" "source/sound-au.c" "source/sound-wav.c" "source/endianness.c"\
   "source/error.c" -I"./include" -DDEBUG -DSTANDALONE_TEST -o "./test"
-----------------------------*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "endianness.h"
#include "sound-local.h"

#define ULAW_BIAS 0x84
#define ALAW_AMI_MASK 0x55


/*-----------------------------

 ReadPcm()
-----------------------------*/
int ReadPcm(FILE* file, struct Sound* sound)
{
	int8_t* dest = sound->data;

	if (fread(dest, sound->size, 1, file) != 1)
		return 1;

	if (sound->format != SOUND_I8)
	{
		enum Endianness sys_endianness = EndianSystem();
		size_t bytes = sound->size / sound->length / sound->channels;
		int8_t* end = (int8_t*)sound->data + sound->size;

		if (sound->format == SOUND_I16)
		{
			for (dest = sound->data; dest < end; dest += (bytes * sound->channels))
				for (size_t c = 0; c < sound->channels; c++)
					((uint16_t*)dest)[c] = EndianBigToSystem_16(((uint16_t*)dest)[c], sys_endianness);
		}
		else if (sound->format == SOUND_I32 || sound->format == SOUND_F32)
		{
			for (dest = sound->data; dest < end; dest += (bytes * sound->channels))
				for (size_t c = 0; c < sound->channels; c++)
					((uint32_t*)dest)[c] = EndianBigToSystem_32(((uint32_t*)dest)[c], sys_endianness);
		}
		else if (sound->format == SOUND_F64)
		{
			for (dest = sound->data; dest < end; dest += (bytes * sound->channels))
				for (size_t c = 0; c < sound->channels; c++)
					((uint64_t*)dest)[c] = EndianBigToSystem_64(((uint64_t*)dest)[c], sys_endianness);
		}
	}

	return 0;
}


/*-----------------------------

 ReadULaw()
-----------------------------*/
int ReadULaw(FILE* file, struct Sound* sound)
{
	int16_t* end = (int16_t*)sound->data + (sound->length * sound->channels);
	uint8_t compressed = 0;
	int t;

	for (int16_t* dest = sound->data; dest < end; dest++)
	{
		if (fread(&compressed, sizeof(uint8_t), 1, file) != 1)
			return 1;

		compressed = ~compressed;
		t = (((compressed & 0x0F) << 3) + ULAW_BIAS) << (((int)compressed & 0x70) >> 4);

		*dest = (int16_t)((compressed & 0x80) ? (ULAW_BIAS - t) : (t - ULAW_BIAS));
	}

	return 0;
}


/*-----------------------------

 ReadALaw()
-----------------------------*/
int ReadALaw(FILE* file, struct Sound* sound)
{
	int16_t* end = (int16_t*)sound->data + (sound->length * sound->channels);
	uint8_t compressed = 0;
	int i, seg;

	for (int16_t* dest = sound->data; dest < end; dest++)
	{
		if (fread(&compressed, sizeof(uint8_t), 1, file) != 1)
			return 1;

		compressed ^= ALAW_AMI_MASK;
		i = ((compressed & 0x0F) << 4);
		seg = (((int)compressed & 0x70) >> 4);

		if (seg)
			i = (i + 0x108) << (seg - 1);
		else
			i += 8;

		*dest = (int16_t)((compressed & 0x80) ? i : -i);
	}

	return 0;
}


/*-----------------------------

 SoundCreate()
-----------------------------*/
struct Sound* SoundCreate(enum SoundFormat format, size_t length, size_t channels, size_t frequency)
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
void SoundDelete(struct Sound* sound) { free(sound); }


/*-----------------------------

 SoundLoad()
-----------------------------*/
struct Sound* SoundLoad(const char* filename, struct Error* e)
{
	FILE* file = NULL;
	struct Sound* sound = NULL;
	uint32_t magic = 0;

	ErrorSet(e, NO_ERROR, NULL, NULL);

	if ((file = fopen(filename, "rb")) == NULL)
	{
		ErrorSet(e, ERROR_FS, "SoundLoad", "'%s'", filename);
		return NULL;
	}

	if (fread(&magic, sizeof(uint32_t), 1, file) != 1)
	{
		ErrorSet(e, ERROR_BROKEN, "SoundLoad", "magic ('%s')", filename);
		goto return_failure;
	}

	fseek(file, 0, SEEK_SET);

	if (CheckMagicAu(magic) == true)
		sound = SoundLoadAu(file, filename, e);
	else if (CheckMagicWav(magic) == true)
		sound = SoundLoadWav(file, filename, e);
	else
	{
		ErrorSet(e, ERROR_UNKNOWN_FORMAT, "SoundLoad", "'%s'", filename);
		goto return_failure;
	}

	// Bye!
	fclose(file);
	return sound;

return_failure:
	fclose(file);
	return NULL;
}


/*-----------------------------

 SoundSaveRaw()
-----------------------------*/
struct Error SoundSaveRaw(struct Sound* sound, const char* filename)
{
	struct Error e = {.code = NO_ERROR};
	FILE* file = NULL;

	if ((file = fopen(filename, "wb")) == NULL)
	{
		ErrorSet(&e, ERROR_FS, "SoundSaveRaw", "'%s'", filename);
		return e;
	}

	if (fwrite(sound->data, sound->size, 1, file) != 1)
	{
		ErrorSet(&e, ERROR_IO, "SoundSaveRaw", "'%s'", filename);
		fclose(file);
		return e;
	}

	fclose(file);
	return e;
}


#ifdef STANDALONE_TEST
int main(int argc, char* argv[])
{
	struct Sound* sound = NULL;
	struct Error e = {0};

	if (argc == 1)
	{
		ErrorSet(&e, ERROR_ARGUMENT, "main", "no input specified");
		goto return_failure;
	}

	if ((sound = SoundLoad(argv[1], &e)) != NULL)
	{
		SoundSaveRaw(sound, "test.raw");
		SoundSaveAu(sound, "test.au");
		SoundSaveWav(sound, "test.wav");
		SoundDelete(sound);
	}
	else
		goto return_failure;

	// Bye
	return EXIT_SUCCESS;

return_failure:
	ErrorPrint(e);
	return EXIT_FAILURE;
}
#endif

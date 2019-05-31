/*-----------------------------

 [sound.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef SOUND_H
#define SOUND_H

	#include <stdio.h>
	#include <stddef.h>

	#include "endianness.h"
	#include "status.h"

	enum SoundFormat
	{
		SOUND_I8 = 0,
		SOUND_I16,
		SOUND_I32,
		SOUND_F32,
		SOUND_F64
	};

	enum SoundCompression
	{
		SOUND_UNCOMPRESSED = 0,
		SOUND_ALAW,
		SOUND_ULAW
	};

	struct Sound
	{
		size_t frequency;
		size_t channels;
		size_t length; // In frames
		size_t size;   // In bytes

		enum SoundFormat format;
		void* data;
	};

	struct SoundEx
	{
		size_t frequency;
		size_t channels;
		size_t length; // In frames
		size_t size;   // In bytes

		enum Endianness endianness;
		enum SoundCompression compression;
		enum SoundFormat format;

		size_t data_offset;
	};

	struct Sound* SoundCreate(enum SoundFormat, size_t length, size_t channels, size_t frequency);
	void SoundDelete(struct Sound* sound);

	struct Sound* SoundLoad(const char* filename, struct Status*);
	struct Status SoundSaveAu(struct Sound* sound, const char* filename);
	struct Status SoundSaveWav(struct Sound* sound, const char* filename);
	struct Status SoundSaveRaw(struct Sound* sound, const char* filename);

	int SoundExLoad(FILE* file, struct SoundEx* out, struct Status*);

#endif

/*-----------------------------

 [sound.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef SOUND_H
#define SOUND_H

	#include <stddef.h>
	#include "error.h"

	enum SoundFormat
	{
		SOUND_I8,
		SOUND_I16,
		SOUND_I32,
		SOUND_F32,
		SOUND_F64
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

	struct Sound* SoundCreate(enum SoundFormat, size_t length, size_t channels, size_t frequency);
	void SoundDelete(struct Sound* sound);

	struct Sound* SoundLoad(const char* filename, struct Error*);
	struct Error SoundSaveAu(struct Sound* sound, const char* filename);
	struct Error SoundSaveWav(struct Sound* sound, const char* filename);
	struct Error SoundSaveRaw(struct Sound* sound, const char* filename);

#endif

/*-----------------------------

 [sound-local.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef SOUND_LOCAL_H
#define SOUND_LOCAL_H

	#include <stdbool.h>
	#include <stdio.h>

	#include "sound.h"
	#include "endianness.h"

	bool CheckMagicAu(uint32_t value);
	bool CheckMagicWav(uint32_t value);

	struct Sound* SoundLoadAu(FILE* file, const char* filename, struct Error* e);
	struct Sound* SoundLoadWav(FILE* file, const char* filename, struct Error* e);

	int ReadPcm(FILE* file, struct Sound* sound, enum Endianness);
	int ReadULaw(FILE* file, struct Sound* sound, enum Endianness);
	int ReadALaw(FILE* file, struct Sound* sound, enum Endianness);

#endif

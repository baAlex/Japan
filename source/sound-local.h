/*-----------------------------

 [sound-local.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef SOUND_LOCAL_H
#define SOUND_LOCAL_H

	#include <stdbool.h>
	#include <stdio.h>

	#include "sound.h"

	bool CheckMagicAu(uint32_t value);
	bool CheckMagicWav(uint32_t value);

	struct Sound* SoundLoadAu(FILE* file, const char* filename, struct Error* e);
	struct Sound* SoundLoadWav(FILE* file, const char* filename, struct Error* e);

	int ReadPcm(FILE* file, struct Sound* sound);
	int ReadULaw(FILE* file, struct Sound* sound);
	int ReadALaw(FILE* file, struct Sound* sound);

#endif

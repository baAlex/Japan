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
	// bool CheckMagicWav(uint16_t value);

	struct Sound* SoundLoadAu(FILE* file, const char* filename, struct Error* e);
	// struct Sound* SoundLoadWav(FILE* file, const char* filename, struct Error* e);

#endif

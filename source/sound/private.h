/*-----------------------------

 [private.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef PRIVATE_H
#define PRIVATE_H

	#include <stdlib.h>
	#include <string.h>

	#include "sound.h"
	#include "endianness.h"

	bool CheckMagicAu(uint32_t value);
	bool CheckMagicWav(uint32_t value);

	int SoundExLoadAu(FILE* file, struct SoundEx* out, struct Status* st);
	int SoundExLoadWav(FILE* file, struct SoundEx* out, struct Status* st);

	int WritePcm(FILE* file, struct Sound* sound, enum Endianness);

	int16_t AlawToInt16(uint8_t compressed);
	int16_t UlawToInt16(uint8_t compressed);

#endif

/*-----------------------------

 [sound/private.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JA_SOUND_PRIVATE_H
#define JA_SOUND_PRIVATE_H

	#include <stdlib.h>
	#include <string.h>

	#include "../common.h"

	#include "japan-sound.h"
	#include "japan-endianness.h"

	bool CheckMagicAu(uint32_t value);
	bool CheckMagicWav(uint32_t value);

	int SoundExLoadAu(FILE* file, struct jaSoundEx* out, struct jaStatus* st);
	int SoundExLoadWav(FILE* file, struct jaSoundEx* out, struct jaStatus* st);

	int WritePcm(FILE* file, const struct jaSound* sound, enum jaEndianness);

	int16_t AlawToInt16(uint8_t compressed);
	int16_t UlawToInt16(uint8_t compressed);

#endif

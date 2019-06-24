/*-----------------------------

 [sound-private.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef SOUND_PRIVATE_H
#define SOUND_PRIVATE_H

	#include <stdlib.h>
	#include <string.h>

	#include "sound.h"
	#include "endianness.h"

	#ifdef DEBUG
	#define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
	#else
	#define DEBUG_PRINT(fmt, ...) // Whitespace
	#endif

	#ifdef EXPORT_SYMBOLS
	#define EXPORT __attribute__((visibility("default")))
	#else
	#define EXPORT // Whitespace
	#endif

	bool CheckMagicAu(uint32_t value);
	bool CheckMagicWav(uint32_t value);

	int SoundExLoadAu(FILE* file, struct SoundEx* out, struct Status* st);
	int SoundExLoadWav(FILE* file, struct SoundEx* out, struct Status* st);

	int WritePcm(FILE* file, struct Sound* sound, enum Endianness);

#endif

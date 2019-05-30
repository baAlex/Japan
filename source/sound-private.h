/*-----------------------------

 [sound-private.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef SOUND_PRIVATE_H
#define SOUND_PRIVATE_H

	#include <stdbool.h>
	#include <stdio.h>

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

	struct Sound* SoundLoadAu(FILE* file, const char* filename, struct Status* st);
	struct Sound* SoundLoadWav(FILE* file, const char* filename, struct Status* st);

	int ReadPcm(FILE* file, struct Sound* sound, enum Endianness);
	int ReadULaw(FILE* file, struct Sound* sound, enum Endianness);
	int ReadALaw(FILE* file, struct Sound* sound, enum Endianness);

	int WritePcm(FILE* file, struct Sound* sound, enum Endianness);

#endif

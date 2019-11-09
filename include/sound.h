/*-----------------------------

 [sound.h]
 - Alexander Brandt 2019
-----------------------------*/

#ifndef JAPAN_VERSION
#define JAPAN_VERSION "0.2.3"
#define JAPAN_VERSION_MAJOR 0
#define JAPAN_VERSION_MINOR 2
#define JAPAN_VERSION_PATCH 3
#endif

#if defined(EXPORT_SYMBOLS) && defined(_WIN32)
#define DLL_EXP __declspec(dllexport)
#else
#define DLL_EXP // Whitespace
#endif

#ifndef SOUND_H
#define SOUND_H

	#include <stdio.h>
	#include <stddef.h>
	#include <stdbool.h>

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
		size_t length;            // In frames
		size_t uncompressed_size; // In bytes
		size_t minimum_unit_size; // In bytes

		enum Endianness endianness;
		enum SoundCompression compression;
		enum SoundFormat format;

		bool unsigned_8bit;
		bool unspecified_size;

		size_t data_offset;
	};

	DLL_EXP struct Sound* SoundCreate(enum SoundFormat, size_t length, size_t channels, size_t frequency);
	DLL_EXP void SoundDelete(struct Sound* sound);

	DLL_EXP struct Sound* SoundLoad(const char* filename, struct Status*);
	DLL_EXP struct Status SoundSaveAu(struct Sound* sound, const char* filename);
	DLL_EXP struct Status SoundSaveWav(struct Sound* sound, const char* filename);
	DLL_EXP struct Status SoundSaveRaw(struct Sound* sound, const char* filename);

	DLL_EXP int SoundExLoad(FILE* file, struct SoundEx* out, struct Status*);
	DLL_EXP size_t SoundExRead(FILE* file, struct SoundEx ex, size_t out_size, void* out, struct Status*);

	DLL_EXP int SoundBps(enum SoundFormat);

#endif

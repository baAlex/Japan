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
#define JAPAN_API __declspec(dllexport)
#else
#define JAPAN_API // Whitespace
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

	JAPAN_API struct Sound* SoundCreate(enum SoundFormat, size_t length, size_t channels, size_t frequency);
	JAPAN_API void SoundDelete(struct Sound* sound);

	JAPAN_API struct Sound* SoundLoad(const char* filename, struct Status*);
	JAPAN_API struct Status SoundSaveAu(struct Sound* sound, const char* filename);
	JAPAN_API struct Status SoundSaveWav(struct Sound* sound, const char* filename);
	JAPAN_API struct Status SoundSaveRaw(struct Sound* sound, const char* filename);

	JAPAN_API int SoundExLoad(FILE* file, struct SoundEx* out, struct Status*);
	JAPAN_API size_t SoundExRead(FILE* file, struct SoundEx ex, size_t out_size, void* out, struct Status*);

	JAPAN_API int SoundBps(enum SoundFormat);

#endif

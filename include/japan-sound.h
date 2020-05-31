/*-----------------------------

 [japan-sound.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_SOUND_H
#define JAPAN_SOUND_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "japan-endianness.h"
#include "japan-status.h"

enum jaSoundStorage
{
	JA_SOUND_UNCOMPRESSED = 0,
	JA_SOUND_ALAW,
	JA_SOUND_ULAW
};

enum jaSoundFormat
{
	JA_SOUND_I8 = 0,
	JA_SOUND_I16,
	JA_SOUND_I32,
	JA_SOUND_F32,
	JA_SOUND_F64
};

struct jaSound
{
	size_t frequency;
	size_t channels;
	size_t length; // In frames
	size_t size;   // In bytes

	enum jaSoundFormat format;
	void* data;
};

struct jaSoundEx
{
	size_t frequency;
	size_t channels;
	size_t length; // In frames, ignored by jaSoundExLoadRaw() if set to 0

	size_t uncompressed_size; // In bytes, ignored by jaSoundExLoadRaw()
	size_t minimum_unit_size; // In bytes, ignored by jaSoundExLoadRaw()

	enum jaEndianness endianness;
	enum jaSoundStorage storage;
	enum jaSoundFormat format;

	bool unsigned_8bit;
	bool unspecified_size; // Ignored by jaSoundExLoadRaw()

	size_t data_offset;
};

JA_EXPORT struct jaSound* jaSoundCreate(enum jaSoundFormat, size_t length, size_t channels, size_t frequency);
JA_EXPORT void jaSoundDelete(struct jaSound* sound);

JA_EXPORT struct jaSound* jaSoundLoad(const char* filename, struct jaStatus*);
JA_EXPORT int jaSoundSaveAu(const struct jaSound* sound, const char* filename, struct jaStatus*);
JA_EXPORT int jaSoundSaveWav(const struct jaSound* sound, const char* filename, struct jaStatus*);
JA_EXPORT int jaSoundSaveRaw(const struct jaSound* sound, const char* filename, struct jaStatus*);

JA_EXPORT struct jaSound* jaSoundLoadRaw(FILE* file, const struct jaSoundEx* in, struct jaStatus*);
JA_EXPORT int jaSoundExLoad(FILE* file, struct jaSoundEx* out, struct jaStatus*);
JA_EXPORT size_t jaSoundExRead(FILE* file, struct jaSoundEx ex, size_t out_size, void* out, struct jaStatus*);

JA_EXPORT int jaBytesPerSample(enum jaSoundFormat);

#endif

/*-----------------------------

 [japan-version.h]
 - Alexander Brandt 2019-2020
-----------------------------*/

#ifndef JAPAN_VERSION_H
#define JAPAN_VERSION_H

#ifdef JA_EXPORT_SYMBOLS
	#if defined(__clang__) || defined(__GNUC__)
	#define JA_EXPORT __attribute__((visibility("default")))
	#elif defined(_MSC_VER)
	#define JA_EXPORT __declspec(dllexport)
	#endif
#else
	#define JA_EXPORT // Whitespace
#endif

JA_EXPORT int jaVersionMajor();
JA_EXPORT int jaVersionMinor();
JA_EXPORT int jaVersionPatch();

JA_EXPORT char* jaVersionString();

#endif

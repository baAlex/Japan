/*-----------------------------

MIT License

Copyright (c) 2019 Alexander Brandt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [sound-wav.c]
 - Alexander Brandt 2019

 http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
-----------------------------*/

#include <stdlib.h>
#include <string.h>

#include "endianness.h"
#include "sound-local.h"

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif


/*-----------------------------

 CheckMagicWav()
-----------------------------*/
bool CheckMagicWav(uint32_t value)
{
	return false;
}


/*-----------------------------

 SoundLoadWav()
-----------------------------*/
struct Sound* SoundLoadWav(FILE* file, const char* filename, struct Error* e)
{
	return NULL;
}


/*-----------------------------

 SoundSaveWav()
-----------------------------*/
struct Error SoundSaveWav(struct Sound* sound, const char* filename)
{
	struct Error e = {.code = NO_ERROR};
	return e;
}

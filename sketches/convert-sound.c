
// $ clang -I../include convert-sound.c ../japan-dbg.a -g -o ./convert-sound

#include <stdlib.h>

#include "error.h"
#include "sound.h"

int main(int argc, char* argv[])
{
	struct Sound* snd = NULL;
	struct Error e = {0};

	if (argc == 1)
	{
		ErrorSet(&e, ERROR_ARGUMENT, "main", "no input specified");
		goto return_failure;
	}

	if ((snd = SoundLoad(argv[1], &e)) != NULL)
	{
		SoundSaveAu(snd, "/tmp/test.au");
		SoundSaveWav(snd, "/tmp/test.wav");
		SoundSaveRaw(snd, "/tmp/test.data");
		SoundDelete(snd);
	}
	else
		goto return_failure;

	// Bye!
	return EXIT_SUCCESS;

return_failure:
	ErrorPrint(e);
	return EXIT_FAILURE;
}


// $ clang -I../include convert-sound.c ../japan-dbg.a -g -O0 -o ./convert-sound

#include <stdlib.h>
#include "sound.h"

int main(int argc, char* argv[])
{
	struct Sound* snd = NULL;
	struct Status st = {0};

	if (argc == 1)
	{
		StatusSet(&st, "main", STATUS_INVALID_ARGUMENT, "no input specified");
		goto return_failure;
	}

	if ((snd = SoundLoad(argv[1], &st)) != NULL)
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
	StatusPrint(st);
	return EXIT_FAILURE;
}

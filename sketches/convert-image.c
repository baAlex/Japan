
// $ clang -I./include sketches/convert-image.c ./japan-dbg.a -O0 -g -o ./convert-image

#include <stdlib.h>
#include "image.h"

int main(int argc, char* argv[])
{
	struct Image* img = NULL;
	struct Status st = {0};

	if (argc == 1)
	{
		StatusSet(&st, "main", STATUS_INVALID_ARGUMENT, "no input specified");
		goto return_failure;
	}

	if ((img = ImageLoad(argv[1], &st)) != NULL)
	{
		ImageSaveSgi(img, "/tmp/test.sgi");
		ImageSaveRaw(img, "/tmp/test.data");
		ImageDelete(img);
	}
	else
		goto return_failure;

	// Bye!
	return EXIT_SUCCESS;

return_failure:
	StatusPrint(st);
	return EXIT_FAILURE;
}

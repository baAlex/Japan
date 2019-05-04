
// $ clang -I../include convert-image.c ../japan-dbg.a -g -o ./convert-image

#include <stdlib.h>

#include "error.h"
#include "image.h"

int main(int argc, char* argv[])
{
	struct Image* img = NULL;
	struct Error e = {0};

	if (argc == 1)
	{
		ErrorSet(&e, ERROR_ARGUMENT, "main", "no input specified");
		goto return_failure;
	}

	if ((img = ImageLoad(argv[1], &e)) != NULL)
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
	ErrorPrint(e);
	return EXIT_FAILURE;
}

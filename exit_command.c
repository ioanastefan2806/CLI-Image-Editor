#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "image.h"
#include "exit_command.h"
#include "error.h"

void exit_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	if (!image)
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (!image->is_loaded)
		longjmp(ex_buf__, E_NO_IMAGE_LOADED);

	if (!argv)
		argc = argc - 1 + 1;

	reset_image(image);
}

#include <stdbool.h>
#include <setjmp.h>
#include <stdio.h>

#include "crop_command.h"
#include "image.h"
#include "error.h"

#define CROP_ARG_COUNT 0
#define CROP_SUCCESS_MSG "Image cropped\n"

void crop_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	if (!image)
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (argc != CROP_ARG_COUNT)
		longjmp(ex_buf__, E_INVALID_COMMAND);

	if (!image->is_loaded)
		longjmp(ex_buf__, E_NO_IMAGE_LOADED);

	// bypass unused parameter warning
	if (!argv)
		argc++;

	size_t new_width  = image->selection.lower_right.x -
						image->selection.upper_left.x;
	size_t new_height = image->selection.lower_right.y -
						image->selection.upper_left.y;

	for (size_t i = 0; i < new_height; i++)
		for (size_t j = 0; j < new_width; j++)
			image->matrix[i][j] =
			image->matrix[i + image->selection.upper_left.y]
						 [j + image->selection.upper_left.x];

	if (resize_matrix(image, new_width, new_height) == -1)
		longjmp(ex_buf__, E_FUNC_FAILED);

	image->selection.upper_left.x = 0;
	image->selection.upper_left.y = 0;
	image->selection.lower_right.x = image->width;
	image->selection.lower_right.y = image->height;

	printf(CROP_SUCCESS_MSG);
}

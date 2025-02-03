#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>

#include "image.h"
#include "rotate_command.h"
#include "error.h"
#include "utils.h"

#define ROTATE_ARG_COUNT 1
#define ROTATE_SUCCESS_MSG "Rotated %d\n"

#define MAX_ROTATE_ANGLE 360
#define MIN_ROTATE_ANGLE (-360)

static int transpose_matrix(image_t *image);
static int transpose_selection(image_t *image);

void rotate_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	if (!image)
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (argc != ROTATE_ARG_COUNT)
		longjmp(ex_buf__, E_INVALID_COMMAND);

	if (!image->is_loaded)
		longjmp(ex_buf__, E_NO_IMAGE_LOADED);

	int temp = atoi(argv[0]);

	// check if argument is a number
	if (!temp && argv[0][0] != '0')
		longjmp(ex_buf__, E_INVALID_COMMAND);

	// check if argument is valid
	if (!(temp % 90 == 0 && temp <= MAX_ROTATE_ANGLE &&
		  temp >= MIN_ROTATE_ANGLE))
		longjmp(ex_buf__, E_UNSUPPORTED_ROT_ANGLE);

	if (!whole_matrix_is_selected(image) && !selection_is_square(image))
		longjmp(ex_buf__, E_SELECTION_NOT_SQUARE);

	int rotation_count = temp / 90;

	if (rotation_count < 0)
		rotation_count += 4;

	for (int i = 0; i < rotation_count; i++) {
		if (whole_matrix_is_selected(image) && transpose_matrix(image) == -1)
			longjmp(ex_buf__, E_FUNC_FAILED);

		if (!whole_matrix_is_selected(image) &&
			transpose_selection(image) == -1)
			longjmp(ex_buf__, E_FUNC_FAILED);
	}

	printf(ROTATE_SUCCESS_MSG, temp);
}

static int transpose_selection(image_t *image)
{
	if (!image)
		return -1;

	size_t size = image->selection.lower_right.x -
				  image->selection.upper_left.x;

	for (size_t i = 0; i < size; i++)
		for (size_t j = i + 1; j < size; j++)
			swap_pixel_t(&image->matrix[i + image->selection.upper_left.y]
									   [j + image->selection.upper_left.x],
						 &image->matrix[j + image->selection.upper_left.y]
									   [i + image->selection.upper_left.x]);

	for (size_t i = 0; i < size; i++)
		for (size_t j = 0; j < size / 2; j++)
			swap_pixel_t(&image->matrix[i + image->selection.upper_left.y]
									   [j + image->selection.upper_left.x],
						 &image->matrix[i + image->selection.upper_left.y]
									   [size - j - 1 +
										image->selection.upper_left.x]);
	return 0;
}

static int transpose_matrix(image_t *image)
{
	if (!image)
		return 0;

	image_t res;
	res.height = image->width;
	res.width  = image->height;
	res.is_loaded = image->is_loaded;
	res.magic_word = image->magic_word;
	res.max_val = image->max_val;
	res.selection.upper_left.x = 0;
	res.selection.upper_left.y = 0;
	res.selection.lower_right.x = res.width;
	res.selection.lower_right.y = res.height;

	if (create_matrix(&res) == -1)
		return -1;

	for (size_t i = 0; i < res.height; i++)
		for (size_t j = 0; j < res.width; j++)
			res.matrix[i][j] = image->matrix[j][i];

	for (size_t i = 0; i < res.height; i++)
		for (size_t j = 0; j < res.width / 2; j++)
			swap_pixel_t(&res.matrix[i][j], &res.matrix[i][res.width - j - 1]);

	if (copy_image(image, &res) == -1)
		return -1;

	reset_image(&res);

	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils.h"
#include "image.h"
/*
 * if given pointer is null, the function prints
 * given error message and stops program execution
 */
void check_nullptr(void *ptr, char error_msg[])
{
	if (!ptr) {
		perror(error_msg);
		exit(EXIT_FAILURE);
	}
}

size_t min(size_t a, size_t b)
{
	return (a > b) ? b : a;
}

double clamp_value(double val)
{
	if (val > MAX_PIXEL_VAL)
		return MAX_PIXEL_VAL;

	if (val < MIN_PIXEL_VAL)
		return MIN_PIXEL_VAL;

	return val;
}

void swap_int(int *a, int *b)
{
	if (!a || !b)
		return;

	int aux = *a;
	*a = *b;
	*b = aux;
}

void swap_pixel_t(pixel_t *a, pixel_t *b)
{
	if (!a || !b)
		return;

	pixel_t aux = *a;
	*a = *b;
	*b = aux;
}

// checks if selected zone refers to the whole image
bool whole_matrix_is_selected(image_t *image)
{
	if (!image)
		return false;

	if (!image->selection.upper_left.x && !image->selection.upper_left.y &&
		image->selection.lower_right.x == image->width &&
		image->selection.lower_right.y == image->height)
		return true;

	return false;
}

// checks if selected zone is square
bool selection_is_square(image_t *image)
{
	if (!image)
		return false;

	if (image->selection.lower_right.x - image->selection.upper_left.x ==
		image->selection.lower_right.y - image->selection.upper_left.y)
		return true;

	return false;
}

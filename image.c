#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "image.h"
#include "utils.h"

// checks if a magic word refers to a binary image
bool is_binary(MAGIC_WORD magic_word)
{
	return (magic_word == P5 || magic_word == P6);
}

// checks if a magic word refers to a color image
bool is_color(MAGIC_WORD magic_word)
{
	return (magic_word == P3 || magic_word == P6);
}

// allocates memory for a pixel matrix
int create_matrix(image_t *image)
{
	if (!image || image->height <= 0 || image->width <= 0)
		return -1;

	image->matrix = calloc(image->height, sizeof(*image->matrix));

	if (!image->matrix)
		return -1;

	for (size_t i = 0; i < image->height; i++) {
		image->matrix[i] = calloc(image->width, sizeof(*image->matrix[i]));

		if (!image->matrix[i]) {
			// free previously allocated memory
			for (size_t j = 0; j < i; j++)
				free(image->matrix[i]);

			free(image->matrix);

			return -1;
		}
	}

	return 0;
}

// frees allocated memory for an image's pixel matrix
void free_matrix(image_t *image)
{
	if (!image || !image->matrix)
		return;

	for (size_t i = 0; i < image->height; i++)
		free(image->matrix[i]);

	free(image->matrix);

	image->matrix = NULL;
}

// resets image and frees allocated memory
void reset_image(image_t *image)
{
	if (!image)
		return;

	free_matrix(image);

	image->magic_word = INVALID_MAGIC_WORD;
	image->height     = 0;
	image->width      = 0;
	image->max_val    = 0;
	image->is_loaded  = false;

	image->selection.lower_right.x = 0;
	image->selection.lower_right.y = 0;
	image->selection.upper_left.x  = 0;
	image->selection.upper_left.y  = 0;
}

// resizes an image's pixel matrix to the new dimensions
int resize_matrix(image_t *image, size_t new_width, size_t new_height)
{
	if (!image || !new_width || !new_height)
		return -1;

	for (size_t i = new_height; i < image->height; i++)
		free(image->matrix[i]);

	void *ret = realloc(image->matrix, new_height * sizeof(*image->matrix));

	if (!ret)
		return -1;

	image->matrix = ret;

	for (size_t i = 0; i < min(new_height, image->height); i++) {
		ret = realloc(image->matrix[i], new_width * sizeof(*image->matrix[i]));

		if (!ret)
			return -1;

		image->matrix[i] = ret;
	}

	for (size_t i = image->height; i < new_height; i++) {
		image->matrix[i] = calloc(new_width, sizeof(*image->matrix[i]));

		if (!image->matrix[i])
			return -1;
	}

	image->width  = new_width;
	image->height = new_height;

	return 0;
}

// copies src image to dest
int copy_image(image_t *dest, image_t *src)
{
	if (!dest || !src)
		return -1;

	reset_image(dest);

	dest->magic_word = src->magic_word;
	dest->height     = src->height;
	dest->width      = src->width;
	dest->max_val    = src->max_val;
	dest->is_loaded  = src->is_loaded;

	dest->selection.upper_left.x  = src->selection.upper_left.x;
	dest->selection.upper_left.y  = src->selection.upper_left.y;
	dest->selection.lower_right.x = src->selection.lower_right.x;
	dest->selection.lower_right.y = src->selection.lower_right.y;

	if (create_matrix(dest) == -1)
		return -1;

	for (size_t i = 0; i < src->height; i++)
		for (size_t j = 0; j < src->width; j++)
			dest->matrix[i][j] = src->matrix[i][j];

	return 0;
}

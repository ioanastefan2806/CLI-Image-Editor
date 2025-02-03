#include <setjmp.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "image.h"
#include "equalize_command.h"
#include "histogram_command.h"
#include "error.h"
#include "utils.h"

#define EQUALIZE_ARG_COUNT 0
#define EQUALIZE_SUCCESS_MSG "Equalize done\n"

void equalize_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	if (!image)
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (!image->is_loaded)
		longjmp(ex_buf__, E_NO_IMAGE_LOADED);

	if (is_color(image->magic_word))
		longjmp(ex_buf__, E_COLOR_IMAGE);

	if (!argv)
		argc += 0;

	size_t freq[MAX_PIXEL_VAL + 1] = {0};
	size_t surface_area = image->height * image->width;

	for (size_t i = 0; i < image->height; i++) {
		for (size_t j = 0; j < image->width; j++) {
			unsigned char val = (unsigned char)round
								(image->matrix[i][j].grayscale.value);

			freq[val]++;
		}
	}

	for (size_t i = 0; i < image->height; i++) {
		for (size_t j = 0; j < image->width; j++) {
			unsigned char val = (unsigned char)round
								(image->matrix[i][j].grayscale.value);
			size_t sum = 0;

			for (size_t k = 0; k <= val; k++)
				sum += freq[k];

			image->matrix[i][j].grayscale.value =
			(double)(MAX_PIXEL_VAL * sum) / surface_area;

			image->matrix[i][j].grayscale.value =
			clamp_value(image->matrix[i][j].grayscale.value);

			val = (unsigned char)round(image->matrix[i][j].grayscale.value);

			if (val > image->max_val)
				image->max_val = val;
		}
	}

	printf(EQUALIZE_SUCCESS_MSG);
}

#include <setjmp.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "image.h"
#include "histogram_command.h"
#include "error.h"

#define HISTOGRAM_ARG_COUNT 2

static int create_histogram(histogram_t *histogram, image_t *image, size_t bins,
							size_t max_val);
static void print_histogram(histogram_t histogram);
static inline void free_histogram(histogram_t histogram);

void histogram_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	if (!image)
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (!image->is_loaded)
		longjmp(ex_buf__, E_NO_IMAGE_LOADED);

	if (argc != HISTOGRAM_ARG_COUNT)
		longjmp(ex_buf__, E_INVALID_COMMAND);

	int temp[2];

	for (int i = 0; i < 2; i++) {
		temp[i] = atoi(argv[i]);

		if (!temp[i] && argv[i][0] != '0')
			longjmp(ex_buf__, E_INVALID_COMMAND);
	}

	if (is_color(image->magic_word))
		longjmp(ex_buf__, E_COLOR_IMAGE);

	histogram_t histogram;

	if (create_histogram(&histogram, image, (size_t)temp[1],
						 (size_t)temp[0]) == -1)
		longjmp(ex_buf__, E_FUNC_FAILED);

	print_histogram(histogram);
	free_histogram(histogram);
}

static int create_histogram(histogram_t *histogram, image_t *image, size_t bins,
							size_t max_val)
{
	if (!image || !histogram)
		return -1;

	histogram->bins = bins;
	histogram->max_val = max_val;

	histogram->values = calloc(bins, sizeof(*histogram->values));

	if (!histogram->values)
		return -1;

	size_t freq[MAX_PIXEL_VAL + 1] = {0};
	size_t max_freq = 0;

	for (size_t i = 0; i < image->height; i++) {
		for (size_t j = 0; j < image->width; j++) {
			unsigned char val = (unsigned char)round
								(image->matrix[i][j].grayscale.value);

			freq[val]++;
		}
	}

	for (size_t i = 0; i <= MAX_PIXEL_VAL; i++) {
		size_t idx = i * bins / (MAX_PIXEL_VAL + 1);

		histogram->values[idx] += freq[i];

		if (histogram->values[idx] > max_freq)
			max_freq = histogram->values[idx];
	}

	for (size_t i = 0; i < bins; i++) {
		histogram->values[i] *= max_val;
		histogram->values[i] = floor((double)histogram->values[i] /
									 (double)max_freq);
	}

	return 0;
}

static void print_histogram(histogram_t histogram)
{
	for (size_t i = 0; i < histogram.bins; i++) {
		printf("%zu\t|\t", histogram.values[i]);

		if (histogram.values[i] > 10000)
			continue;

		for (size_t j = 0; j < histogram.values[i]; j++)
			printf("*");

		printf("\n");
	}
}

static inline void free_histogram(histogram_t histogram)
{
	free(histogram.values);
}

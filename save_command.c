#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "save_command.h"
#include "error.h"
#include "image.h"

#define SAVE_MIN_ARG_COUNT 1
#define SAVE_MAX_ARG_COUNT 2

#define SAVE_SUCCESS_MSG "Saved %s\n"

static void save_ascii_matrix(FILE *fp, image_t *image);
static void save_color_ascii_pixel(FILE *fp, pixel_t pixel);
static void save_grayscale_ascii_pixel(FILE *fp, pixel_t pixel);
static void save_binary_matrix(FILE *fp, image_t *image);
static void save_color_binary_pixel(FILE *fp, pixel_t pixel);
static void save_grayscale_binary_pixel(FILE *fp, pixel_t pixel);

void save_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	if (!image || !argv || !(*argv))
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (argc < SAVE_MIN_ARG_COUNT || argc > SAVE_MAX_ARG_COUNT)
		longjmp(ex_buf__, E_INVALID_COMMAND);

	if (argc == 2 && strcmp(argv[1], "ascii"))
		longjmp(ex_buf__, E_INVALID_COMMAND);

	if (!image->is_loaded)
		longjmp(ex_buf__, E_NO_IMAGE_LOADED);

	FILE *fp = fopen(argv[0], "w");

	if (!fp)
		longjmp(ex_buf__, E_FUNC_FAILED);

	if (is_color(image->magic_word)) {
		if (argc == 2)
			fprintf(fp, "%s\n", magic_word_to_str(P3));
		else
			fprintf(fp, "%s\n", magic_word_to_str(P6));
	} else {
		if (argc == 2)
			fprintf(fp, "%s\n", magic_word_to_str(P2));
		else
			fprintf(fp, "%s\n", magic_word_to_str(P5));
	}

	fprintf(fp, "%zu %zu\n", image->width, image->height);
	fprintf(fp, "%hhu\n", image->max_val);

	if (argc == 1) {
		fp = freopen(argv[0], "ab", fp);

		if (!fp)
			longjmp(ex_buf__, E_FUNC_FAILED);

		save_binary_matrix(fp, image);
	} else {
		save_ascii_matrix(fp, image);
	}

	fclose(fp);

	printf(SAVE_SUCCESS_MSG, argv[0]);
}

static void save_ascii_matrix(FILE *fp, image_t *image)
{
	if (!fp || !image)
		return;

	for (size_t i = 0; i < image->height; i++) {
		for (size_t j = 0; j < image->width; j++)
			if (is_color(image->magic_word))
				save_color_ascii_pixel(fp, image->matrix[i][j]);
			else
				save_grayscale_ascii_pixel(fp, image->matrix[i][j]);

		fprintf(fp, "\n");
	}
}

static void save_color_ascii_pixel(FILE *fp, pixel_t pixel)
{
	if (!fp)
		return;

	fprintf(fp, "%hhu %hhu %hhu ", (unsigned char)round(pixel.color.red),
			(unsigned char)round(pixel.color.green),
			(unsigned char)round(pixel.color.blue));
}

static void save_grayscale_ascii_pixel(FILE *fp, pixel_t pixel)
{
	if (!fp)
		return;

	fprintf(fp, "%hhu ", (unsigned char)round(pixel.grayscale.value));
}

static void save_binary_matrix(FILE *fp, image_t *image)
{
	if (!fp || !image)
		return;

	for (size_t i = 0; i < image->height; i++)
		for (size_t j = 0; j < image->width; j++)
			if (is_color(image->magic_word))
				save_color_binary_pixel(fp, image->matrix[i][j]);
			else
				save_grayscale_binary_pixel(fp, image->matrix[i][j]);
}

static void save_color_binary_pixel(FILE *fp, pixel_t pixel)
{
	if (!fp)
		return;

	unsigned char temp[3];

	temp[0] = (unsigned char)round(pixel.color.red);
	temp[1] = (unsigned char)round(pixel.color.green);
	temp[2] = (unsigned char)round(pixel.color.blue);

	fwrite(temp, sizeof(unsigned char), 3, fp);
}

static void save_grayscale_binary_pixel(FILE *fp, pixel_t pixel)
{
	if (!fp)
		return;

	unsigned char temp = (unsigned char)round(pixel.grayscale.value);

	fwrite(&temp, sizeof(unsigned char), 1, fp);
}

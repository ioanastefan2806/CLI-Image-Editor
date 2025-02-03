#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdbool.h>

#include "load_command.h"
#include "image.h"
#include "error.h"
#include "utils.h"

#define LOAD_ARG_COUNT 1
#define LOAD_SUCCESS_MSG "Loaded %s\n"

static int read_image(FILE *fp, image_t *image, char *filename);
static int read_magic_word(FILE *fp, image_t *image);
static int read_size(FILE *fp, image_t *image);
static int read_max_val(FILE *fp, image_t *image);
static int read_ascii_matrix(FILE *fp, image_t *image);
static int read_grayscale_ascii_pixel(FILE *fp, pixel_t *pixel);
static int read_color_ascii_pixel(FILE *fp, pixel_t *pixel);
static int read_binary_matrix(FILE *fp, image_t *image);
static int read_color_binary_pixel(FILE *fp, pixel_t *pixel);
static int read_grayscale_binary_pixel(FILE *fp, pixel_t *pixel);
static void ignore_comments(FILE *fp);

void load_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	// check function arguments
	if (!image || !argv || !(*argv))
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (image->is_loaded)
		reset_image(image);

	// check command arguments
	if (argc != LOAD_ARG_COUNT)
		longjmp(ex_buf__, E_INVALID_COMMAND);

	FILE *fp = fopen(argv[0], "r");

	if (!fp)
		longjmp(ex_buf__, E_LOAD_FAILED);

	if (read_image(fp, image, argv[0]) == -1)
		longjmp(ex_buf__, E_LOAD_FAILED);

	image->is_loaded = true;

	// select whole image
	image->selection.upper_left.x  = 0;
	image->selection.upper_left.y  = 0;
	image->selection.lower_right.x = image->width;
	image->selection.lower_right.y = image->height;

	fclose(fp);

	// print success message
	printf(LOAD_SUCCESS_MSG, argv[0]);
}

static int read_image(FILE *fp, image_t *image, char *filename)
{
	// check arguments
	if (!fp || !image)
		return -1;

	if (read_magic_word(fp, image) == -1)
		return -1;

	if (read_size(fp, image) == -1)
		return -1;

	if (read_max_val(fp, image) == -1)
		return -1;

	ignore_comments(fp);

	if (create_matrix(image) == -1)
		return -1;

	// reopen file as binary if necessary
	if (is_binary(image->magic_word)) {
		long pos = ftell(fp);

		fp = freopen(filename, "rb", fp);

		if (!fp)
			return -1;

		fseek(fp, pos + 1, SEEK_SET);

		if (read_binary_matrix(fp, image) == -1)
			return -1;

	} else if (read_ascii_matrix(fp, image) == -1) {
		return -1;
	}

	return 0;
}

static int read_magic_word(FILE *fp, image_t *image)
{
	// check arguments
	if (!fp || !image)
		return -1;

	ignore_comments(fp);

	char magic_word_str[MAX_MAGIC_WORD_LENGTH + 1];

	// reads MAX_MAGIC_WORD_LENGTH characters
	fscanf(fp, "%" STR_VALUE(MAX_MAGIC_WORD_LENGTH) "s", magic_word_str);

	image->magic_word = str_to_magic_word(magic_word_str);

	if (image->magic_word == INVALID_MAGIC_WORD)
		return -1;

	return 0;
}

static int read_size(FILE *fp, image_t *image)
{
	// check arguments
	if (!fp || !image)
		return -1;

	ignore_comments(fp);

	int buffer;

	if (fscanf(fp, "%d", &buffer) == EOF || buffer <= 0)
		return -1;

	image->width = buffer;

	if (fscanf(fp, "%d", &buffer) == EOF || buffer <= 0)
		return -1;

	image->height = buffer;

	return 0;
}

static int read_max_val(FILE *fp, image_t *image)
{
	// check arguments
	if (!fp || !image)
		return -1;

	ignore_comments(fp);

	int buffer;

	if (fscanf(fp, "%d", &buffer) == EOF || buffer < MIN_PIXEL_VAL ||
		buffer > MAX_PIXEL_VAL)
		return -1;

	image->max_val = buffer;

	return 0;
}

static int read_ascii_matrix(FILE *fp, image_t *image)
{
	// check arguments
	if (!fp || !image)
		return -1;

	for (size_t i = 0; i < image->height; i++)
		for (size_t j = 0; j < image->width; j++)
			if ((is_color(image->magic_word) &&
				 read_color_ascii_pixel(fp, &image->matrix[i][j]) == -1) ||
				(!is_color(image->magic_word) &&
				 read_grayscale_ascii_pixel(fp, &image->matrix[i][j]) == -1))
				return -1;

	return 0;
}

static int read_grayscale_ascii_pixel(FILE *fp, pixel_t *pixel)
{
	if (!fp || !pixel)
		return -1;

	if (fscanf(fp, "%lf", &pixel->grayscale.value) == EOF)
		return -1;

	return 0;
}

static int read_color_ascii_pixel(FILE *fp, pixel_t *pixel)
{
	if (!fp || !pixel)
		return -1;

	if (fscanf(fp, "%lf%lf%lf", &pixel->color.red, &pixel->color.green,
			   &pixel->color.blue) == EOF)
		return -1;

	return 1;
}

static int read_binary_matrix(FILE *fp, image_t *image)
{
	// check arguments
	if (!fp || !image)
		return -1;

	for (size_t i = 0; i < image->height; i++)
		for (size_t j = 0; j < image->width; j++)
			if ((is_color(image->magic_word) &&
				 read_color_binary_pixel(fp, &image->matrix[i][j]) == -1) ||
				(!is_color(image->magic_word) &&
				 read_grayscale_binary_pixel(fp, &image->matrix[i][j]) == -1))
				return -1;

	return 0;
}

static int read_color_binary_pixel(FILE *fp, pixel_t *pixel)
{
	if (!fp || !pixel)
		return -1;

	unsigned char buffer[3];

	if (fread(buffer, sizeof(unsigned char), 3, fp) != 3)
		return -1;

	pixel->color.red   = buffer[0];
	pixel->color.green = buffer[1];
	pixel->color.blue  = buffer[2];

	return 0;
}

static int read_grayscale_binary_pixel(FILE *fp, pixel_t *pixel)
{
	if (!fp || !pixel)
		return -1;

	unsigned char buffer;

	if (fread(&buffer, sizeof(buffer), 1, fp) != 1)
		return -1;

	pixel->grayscale.value = buffer;

	return 0;
}

static void ignore_comments(FILE *fp)
{
	// check arguments
	if (!fp)
		return;

	char *buffer = NULL;
	size_t bufsize = 0;

	// read all lines starting with #
	while (fgetc(fp) == '#')
		getline(&buffer, &bufsize, fp);

	// "undo" last fgetc()
	fseek(fp, -1, SEEK_CUR);

	free(buffer);
}

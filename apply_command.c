#include <stdio.h>
#include <setjmp.h>
#include <stdbool.h>

#include "error.h"
#include "apply_command.h"
#include "utils.h"

#define APPLY_ARG_COUNT 1
#define APPLY_SUCCESS_MSG "APPLY %s done\n"
#define KERNEL_SIZE 3

typedef enum {
	INVALID_APPLY_PARAM,
	EDGE,
	SHARPEN,
	BLUR,
	GAUSSIAN_BLUR
} APPLY_PARAM;

static int apply_filter(image_t *image, APPLY_PARAM apply_param);
static int apply_edge(image_t *image);
static int apply_sharpen(image_t *image);
static int apply_blur(image_t *image);
static int apply_gaussian_blur(image_t *image);
static int apply_kernel(image_t *image, const double kernel[][KERNEL_SIZE]);

static inline APPLY_PARAM str_to_apply_param(const char *str)
{
	if (!str)
		return INVALID_APPLY_PARAM;

	static const struct {
		APPLY_PARAM param;
		const char *str;
	} conversion[] = {
		{EDGE, "EDGE"},
		{SHARPEN, "SHARPEN"},
		{BLUR, "BLUR"},
		{GAUSSIAN_BLUR, "GAUSSIAN_BLUR"}
	};

	// bypass check-style warning
	unsigned int size = sizeof(conversion);
	size /= sizeof(conversion[0]);

	for (unsigned int i = 0; i < size; i++)
		if (!strcmp(str, conversion[i].str))
			return conversion[i].param;

	return INVALID_APPLY_PARAM;
}

static inline const char *apply_param_to_str(APPLY_PARAM apply_param)
{
	if (apply_param == INVALID_APPLY_PARAM)
		return NULL;

	static const struct {
		APPLY_PARAM param;
		const char *str;
	} conversion[] = {
		{EDGE, "EDGE"},
		{SHARPEN, "SHARPEN"},
		{BLUR, "BLUR"},
		{GAUSSIAN_BLUR, "GAUSSIAN_BLUR"}
	};

	// bypass check-style warning
	unsigned int size = sizeof(conversion);
	size /= sizeof(conversion[0]);

	for (unsigned int i = 0; i < size; i++)
		if (apply_param == conversion[i].param)
			return conversion[i].str;

	return NULL;
}

void apply_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	if (!image)
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (!image->is_loaded)
		longjmp(ex_buf__, E_NO_IMAGE_LOADED);

	if (argc != APPLY_ARG_COUNT)
		longjmp(ex_buf__, E_INVALID_COMMAND);

	APPLY_PARAM apply_param = str_to_apply_param(argv[0]);

	if (apply_param == INVALID_APPLY_PARAM)
		longjmp(ex_buf__, E_INVALID_APPLY_PARAM);

	if (!is_color(image->magic_word))
		longjmp(ex_buf__, E_GRAYSCALE_IMAGE);

	if (apply_filter(image, apply_param) == -1)
		longjmp(ex_buf__, E_FUNC_FAILED);

	printf(APPLY_SUCCESS_MSG, apply_param_to_str(apply_param));
}

static int apply_filter(image_t *image, APPLY_PARAM apply_param)
{
	if (!image)
		return -1;

	switch (apply_param) {
	case EDGE:
		return apply_edge(image);
	case SHARPEN:
		return apply_sharpen(image);
	case BLUR:
		return apply_blur(image);
	case GAUSSIAN_BLUR:
		return apply_gaussian_blur(image);
	default:
		return -1;
	}
}

static int apply_edge(image_t *image)
{
	if (!image)
		return -1;

	static const double edge_kernel[][KERNEL_SIZE] = {
		{-1.0, -1.0, -1.0},
		{-1.0,    8, -1.0},
		{-1.0, -1.0, -1.0}
	};

	return apply_kernel(image, edge_kernel);
}

static int apply_sharpen(image_t *image)
{
	if (!image)
		return -1;

	static const double sharpen_kernel[][KERNEL_SIZE] = {
		{   0, -1.0,    0},
		{-1.0,    5, -1.0},
		{   0, -1.0,    0}
	};

	return apply_kernel(image, sharpen_kernel);
}

static int apply_blur(image_t *image)
{
	if (!image)
		return -1;

	static const double blur_kernel[][KERNEL_SIZE] = {
		{1.0 / 9, 1.0 / 9, 1.0 / 9},
		{1.0 / 9, 1.0 / 9, 1.0 / 9},
		{1.0 / 9, 1.0 / 9, 1.0 / 9}
	};

	return apply_kernel(image, blur_kernel);
}

static int apply_gaussian_blur(image_t *image)
{
	if (!image)
		return -1;

	static const double gaussian_blur_kernel[][KERNEL_SIZE] = {
		{1.0 / 16, 1.0 / 8, 1.0 / 16},
		{1.0 /  8, 1.0 / 4, 1.0 /  8},
		{1.0 / 16, 1.0 / 8, 1.0 / 16}
	};

	return apply_kernel(image, gaussian_blur_kernel);
}

static int apply_kernel(image_t *image, const double kernel[][KERNEL_SIZE])
{
	image_t res;

	res.matrix = NULL;

	if (copy_image(&res, image) == -1)
		return -1;

	for (size_t i = image->selection.upper_left.y;
		 i < image->selection.lower_right.y; i++) {
		for (size_t j = image->selection.upper_left.x;
			 j < image->selection.lower_right.x; j++) {
			// pixel can't be processed
			if (!i || !j || i == image->height - 1 || j == image->width - 1)
				continue;

			// process pixel
			pixel_t processed_pixel;

			processed_pixel.color.red   = 0;
			processed_pixel.color.green = 0;
			processed_pixel.color.blue  = 0;

			for (size_t k = 0; k < KERNEL_SIZE; k++) {
				for (size_t l = 0; l < KERNEL_SIZE; l++) {
					processed_pixel.color.red +=
					image->matrix[i - 1 + k][j - 1 + l].color.red *
					kernel[k][l];

					processed_pixel.color.green +=
					image->matrix[i - 1 + k][j - 1 + l].color.green *
					kernel[k][l];

					processed_pixel.color.blue +=
					image->matrix[i - 1 + k][j - 1 + l].color.blue *
					kernel[k][l];
				}
			}

			processed_pixel.color.red =
			clamp_value(processed_pixel.color.red);

			processed_pixel.color.green =
			clamp_value(processed_pixel.color.green);

			processed_pixel.color.blue =
			clamp_value(processed_pixel.color.blue);

			if (processed_pixel.color.red > res.max_val)
				res.max_val = processed_pixel.color.red;

			if (processed_pixel.color.green > res.max_val)
				res.max_val = processed_pixel.color.green;

			if (processed_pixel.color.blue > res.max_val)
				res.max_val = processed_pixel.color.blue;

			res.matrix[i][j] = processed_pixel;
		}
	}

	if (copy_image(image, &res) == -1)
		return -1;

	reset_image(&res);

	return 0;
}

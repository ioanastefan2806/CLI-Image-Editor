#include <setjmp.h>
#include <stdio.h>
#include <stdbool.h>

#include "select_command.h"
#include "image.h"
#include "error.h"
#include "utils.h"

#define SELECT_MIN_ARG_COUNT 1
#define SELECT_MAX_ARG_COUNT 4

#define SELECT_ALL_SUCCESS_MSG "Selected ALL\n"
#define SELECT_SUCCESS_MSG "Selected %d %d %d %d\n"

static void select_zone(image_t *image, int coord[4]);
static void select_all(image_t *image);

void select_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__)
{
	if (!image || !argv || !(*argv))
		longjmp(ex_buf__, E_INVALID_FUNC_ARGS);

	if (!((argc == SELECT_MIN_ARG_COUNT && !strcmp(argv[0], "ALL")) ||
		  argc == SELECT_MAX_ARG_COUNT))
		longjmp(ex_buf__, E_INVALID_COMMAND);

	if (!image->is_loaded)
		longjmp(ex_buf__, E_NO_IMAGE_LOADED);

	if (argc == SELECT_MIN_ARG_COUNT) {
		select_all(image);
		printf(SELECT_ALL_SUCCESS_MSG);

		return;
	}

	int coord[4];

	for (int i = 0; i < 4; i++) {
		coord[i] = atoi(argv[i]);

		if (!coord[i] && argv[i][0] != '0')
			longjmp(ex_buf__, E_INVALID_COMMAND);

		if (coord[i] < 0)
			longjmp(ex_buf__, E_INVALID_COORD_SET);
	}

	if ((size_t)coord[0] > image->width || (size_t)coord[2] > image->width ||
		(size_t)coord[1] > image->height || (size_t)coord[3] > image->height)
		longjmp(ex_buf__, E_INVALID_COORD_SET);

	if (coord[0] == coord[2] || coord[1] == coord[3])
		longjmp(ex_buf__, E_INVALID_COORD_SET);

	if (coord[0] > coord[2])
		swap_int(&coord[0], &coord[2]);

	if (coord[1] > coord[3])
		swap_int(&coord[1], &coord[3]);

	select_zone(image, coord);

	printf(SELECT_SUCCESS_MSG, coord[0], coord[1], coord[2], coord[3]);
}

static void select_zone(image_t *image, int coord[4])
{
	if (!image || !coord)
		return;

	image->selection.upper_left.x = coord[0];
	image->selection.upper_left.y = coord[1];

	image->selection.lower_right.x = coord[2];
	image->selection.lower_right.y = coord[3];
}

static void select_all(image_t *image)
{
	if (!image)
		return;

	image->selection.upper_left.x = 0;
	image->selection.upper_left.y = 0;

	image->selection.lower_right.x = image->width;
	image->selection.lower_right.y = image->height;
}

#pragma once

#include <setjmp.h>

#include "image.h"

typedef struct {
	size_t bins;
	size_t max_val;
	size_t *values;
} histogram_t;

void histogram_command(image_t *image, char **argv, int argc,
					   jmp_buf ex_buf__);

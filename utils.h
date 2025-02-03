#pragma once

#include <stdbool.h>

#include "image.h"

#define STR_VALUE(x) STR(x)
#define STR(x) #x

void check_nullptr(void *p, char error_msg[]);

size_t min(size_t a, size_t b);

double clamp_value(double val);

void swap_int(int *a, int *b);

bool whole_matrix_is_selected(image_t *image);

bool selection_is_square(image_t *image);

void swap_pixel_t(pixel_t *a, pixel_t *b);

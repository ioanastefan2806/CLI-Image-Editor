#pragma once

#include <setjmp.h>

#include "image.h"

void apply_command(image_t *image, char **argv, int argc, jmp_buf ex_buf__);

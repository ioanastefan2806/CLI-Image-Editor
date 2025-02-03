#pragma once

#include <string.h>
#include <setjmp.h>

#include "image.h"

#define MAX_COMMAND_TYPE_LENGTH 15
#pragma once

char *parse_command(void);

void run_command(char *command, image_t *image);

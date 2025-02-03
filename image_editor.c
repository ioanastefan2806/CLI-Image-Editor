#include <stdio.h>
#include <stdbool.h>

#include "image.h"
#include "command.h"

int main(void)
{
	image_t loaded_image;
	loaded_image.is_loaded = false;

	char *command = NULL;

	// loop for parsing and running commands
	while ((command = parse_command()) != NULL) {
		// save pointer to free after strtok modifications
		char *og_command = command;

		run_command(command, &loaded_image);

		free(og_command);
		command = NULL;
	}

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "command.h"
#include "error.h"
#include "utils.h"
#include "load_command.h"
#include "save_command.h"
#include "exit_command.h"
#include "select_command.h"
#include "crop_command.h"
#include "apply_command.h"
#include "equalize_command.h"
#include "histogram_command.h"
#include "rotate_command.h"

typedef enum {
	LOAD,
	SELECT,
	HISTOGRAM,
	EQUALIZE,
	ROTATE,
	CROP,
	APPLY,
	SAVE,
	EXIT,
	INVALID_COMMAND_TYPE
} COMMAND_TYPE;

static COMMAND_TYPE get_command_type(char *command);
static COMMAND_TYPE str_to_command_type(const char *str);

// parses command from stdin
char *parse_command(void)
{
	char *buffer = NULL;
	size_t bufsize = 0;

	ssize_t ret = getline(&buffer, &bufsize, stdin);

	if (ret == -1)
		return NULL;

	// remove newline from buffer
	buffer[strcspn(buffer, "\n")] = 0;

	return buffer;
}

// returns command type from a command string
static inline COMMAND_TYPE get_command_type(char *command)
{
	char *token = strtok(command, " ");

	return str_to_command_type(token);
}

// converts string into COMMAND_TYPE enum
static inline COMMAND_TYPE str_to_command_type(const char *str)
{
	if (!str)
		return INVALID_COMMAND_TYPE;

	static const struct {
		COMMAND_TYPE type;
		const char *str;
	} conversion[] = {
		{LOAD, "LOAD"},
		{SELECT, "SELECT"},
		{HISTOGRAM, "HISTOGRAM"},
		{EQUALIZE, "EQUALIZE"},
		{ROTATE, "ROTATE"},
		{CROP, "CROP"},
		{APPLY, "APPLY"},
		{SAVE, "SAVE"},
		{EXIT, "EXIT"}
	};

	// bypass check-style warning
	unsigned int size = sizeof(conversion);
	size /= sizeof(conversion[0]);

	for (unsigned int i = 0; i < size; i++)
		if (!strcmp(str, conversion[i].str))
			return conversion[i].type;

	return INVALID_COMMAND_TYPE;
}

// helper function for running a command
static void __run_command(char *command, image_t *image,
						  void (*func)(image_t *, char **, int, jmp_buf))
{
	jmp_buf ex_buf__;

	char **argv = NULL;
	int argc = 0;

	// split the command into arguments
	while (command && (command = strtok(NULL, " ")) != NULL) {
		void *ret = realloc(argv, ++argc * sizeof(*argv));
		check_nullptr(ret, "realloc() failed");

		argv = ret;

		argv[argc - 1] = calloc(strlen(command) + 1,  sizeof(*argv[argc - 1]));
		check_nullptr(argv[argc - 1], "calloc() failed");

		strcpy(argv[argc - 1], command);
	}

	// basically a try-catch
	switch (setjmp(ex_buf__)) {
	case 0:
	while (1) {
		func(image, argv, argc, ex_buf__);
		break;
	case E_LOAD_FAILED:
		printf("%s %s\n", error_code_to_msg(E_LOAD_FAILED), argv[0]);
		break;
	case E_INVALID_COORD_SET:
		printf("%s\n", error_code_to_msg(E_INVALID_COORD_SET));
		break;
	case E_NO_IMAGE_LOADED:
		printf("%s\n", error_code_to_msg(E_NO_IMAGE_LOADED));
		break;
	case E_SELECTION_NOT_SQUARE:
		printf("%s\n", error_code_to_msg(E_SELECTION_NOT_SQUARE));
		break;
	case E_UNSUPPORTED_ROT_ANGLE:
		printf("%s\n", error_code_to_msg(E_UNSUPPORTED_ROT_ANGLE));
		break;
	case E_COLOR_IMAGE:
		printf("%s\n", error_code_to_msg(E_COLOR_IMAGE));
		break;
	case E_GRAYSCALE_IMAGE:
		printf("%s\n", error_code_to_msg(E_GRAYSCALE_IMAGE));
		break;
	case E_INVALID_APPLY_PARAM:
		printf("%s\n", error_code_to_msg(E_INVALID_APPLY_PARAM));
		break;
	case E_INVALID_HISTOGRAM_PARAM:
		printf("%s\n", error_code_to_msg(E_INVALID_HISTOGRAM_PARAM));
		break;
	case E_INVALID_COMMAND:
		printf("%s\n", error_code_to_msg(E_INVALID_COMMAND));
		break;
	}
	}

	for (int i = 0; i < argc; i++)
		free(argv[i]);

	free(argv);
}

// runs the given command on the given image
void run_command(char *command, image_t *image)
{
	// save pointer to free after strtok modifications
	char *og_command = command;

	switch (get_command_type(command)) {
	case LOAD:
		__run_command(command, image, load_command);
		break;
	case SELECT:
		__run_command(command, image, select_command);
		break;
	case HISTOGRAM:
		__run_command(command, image, histogram_command);
		break;
	case EQUALIZE:
		__run_command(command, image, equalize_command);
		break;
	case ROTATE:
		__run_command(command, image, rotate_command);
		break;
	case CROP:
		__run_command(command, image, crop_command);
		break;
	case APPLY:
		__run_command(command, image, apply_command);
		break;
	case SAVE:
		__run_command(command, image, save_command);
		break;
	case EXIT:
		free(og_command);
		__run_command(NULL, image, exit_command);
		exit(EXIT_SUCCESS);
	default:
		printf("%s\n", error_code_to_msg(E_INVALID_COMMAND));
		break;
	}
}

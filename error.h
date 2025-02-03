#pragma once

typedef enum {
	E_INVALID_COMMAND = 1,
	E_LOAD_FAILED,
	E_INVALID_COORD_SET,
	E_NO_IMAGE_LOADED,
	E_SELECTION_NOT_SQUARE,
	E_UNSUPPORTED_ROT_ANGLE,
	E_COLOR_IMAGE,
	E_GRAYSCALE_IMAGE,
	E_INVALID_APPLY_PARAM,
	E_INVALID_HISTOGRAM_PARAM,
	E_INVALID_FUNC_ARGS,
	E_FUNC_FAILED
} ERROR_CODE;

static inline const char *error_code_to_msg(ERROR_CODE error_code)
{
	static const char * const error_msg[] = {
		[E_INVALID_COMMAND]         = "Invalid command",
		[E_LOAD_FAILED]             = "Failed to load",
		[E_INVALID_COORD_SET]       = "Invalid set of coordinates",
		[E_NO_IMAGE_LOADED]         = "No image loaded",
		[E_SELECTION_NOT_SQUARE]    = "The selection must be square",
		[E_UNSUPPORTED_ROT_ANGLE]   = "Unsupported rotation angle",
		[E_COLOR_IMAGE]             = "Black and white image needed",
		[E_GRAYSCALE_IMAGE]         = "Easy, Charlie Chaplin",
		[E_INVALID_APPLY_PARAM]     = "APPLY parameter invalid",
		[E_INVALID_HISTOGRAM_PARAM] = "Invalid set of parameters",
		[E_INVALID_FUNC_ARGS]       = "Invalid function arguments",
		[E_FUNC_FAILED]             = "Function failed"
	};

	return error_msg[error_code];
}

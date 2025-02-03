#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_MAGIC_WORD_LENGTH 2
#define MAX_PIXEL_VAL 255
#define MIN_PIXEL_VAL 0

typedef enum {
	P2,
	P3,
	P5,
	P6,
	INVALID_MAGIC_WORD
} MAGIC_WORD;

// converts string into MAGIC_WORD enum
static inline MAGIC_WORD str_to_magic_word(const char *str)
{
	if (!str)
		return INVALID_MAGIC_WORD;

	static const struct {
		MAGIC_WORD val;
		const char *str;
	} conversion[] = {
		{P2, "P2"},
		{P3, "P3"},
		{P5, "P5"},
		{P6, "P6"}
	};

	// bypass check-style warning
	unsigned int size = sizeof(conversion);
	size /= sizeof(conversion[0]);

	for (unsigned int i = 0; i < size; i++)
		if (!strcmp(str, conversion[i].str))
			return conversion[i].val;

	return INVALID_MAGIC_WORD;
}

// converts MAGIC_WORD enum to string
static inline const char *magic_word_to_str(MAGIC_WORD magic_word)
{
	static const struct {
		MAGIC_WORD val;
		const char *str;
	} conversion[] = {
		{P2, "P2"},
		{P3, "P3"},
		{P5, "P5"},
		{P6, "P6"}
	};

	// bypass check-style warning
	unsigned int size = sizeof(conversion);
	size /= sizeof(conversion[0]);

	for (unsigned int i = 0; i < size; i++)
		if (magic_word == conversion[i].val)
			return conversion[i].str;

	return NULL;
}

typedef struct {
	double red;
	double green;
	double blue;
} color_pixel_t;

typedef struct {
	double value;
} grayscale_pixel_t;

typedef union {
	color_pixel_t color;
	grayscale_pixel_t grayscale;
} pixel_t;

typedef struct {
	size_t x;
	size_t y;
} point_t;

typedef struct {
	point_t upper_left;
	point_t lower_right;
} selection_t;

typedef struct {
	MAGIC_WORD magic_word;
	size_t width;
	size_t height;
	unsigned char max_val;
	pixel_t **matrix;
	selection_t selection;
	bool is_loaded;
} image_t;

bool is_binary(MAGIC_WORD magic_word);

bool is_color(MAGIC_WORD magic_word);

int create_matrix(image_t *image);

void free_matrix(image_t *image);

void reset_image(image_t *image);

int resize_matrix(image_t *image, size_t new_width, size_t new_height);

int copy_image(image_t *dest, image_t *src);

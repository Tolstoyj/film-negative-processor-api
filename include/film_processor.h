/*
 * Film Processor Library Header
 * Core image processing functions for film negative effects
 */

#ifndef FILM_PROCESSOR_H
#define FILM_PROCESSOR_H

#include <stddef.h>

// Processing modes
typedef enum {
    MODE_TO_NEGATIVE,
    MODE_TO_POSITIVE
} ProcessMode;

// Result structure
typedef struct {
    unsigned char *data;
    int width;
    int height;
    int channels;
    int success;
    char error_message[256];
} ImageResult;

// Main processing function
ImageResult process_image(const unsigned char *input_data, size_t input_size, ProcessMode mode);

// Free image result
void free_image_result(ImageResult *result);

#endif // FILM_PROCESSOR_H

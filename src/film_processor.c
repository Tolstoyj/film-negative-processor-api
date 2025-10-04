/*
 * Film Processor Library Implementation
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "film_processor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Clamp value between 0 and 255
static unsigned char clamp(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (unsigned char)value;
}

// Generate random number for grain
static int random_grain(int range) {
    return (rand() % (range * 2 + 1)) - range;
}

// Invert colors
static void apply_negative(unsigned char *img, int width, int height, int channels) {
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;
        img[idx] = 255 - img[idx];
        img[idx + 1] = 255 - img[idx + 1];
        img[idx + 2] = 255 - img[idx + 2];
    }
}

// Apply orange color cast
static void apply_film_color_cast(unsigned char *img, int width, int height, int channels) {
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;
        unsigned char r = img[idx];
        unsigned char g = img[idx + 1];
        unsigned char b = img[idx + 2];

        img[idx] = clamp((int)(r * 1.15f + 20));
        img[idx + 1] = clamp((int)(g * 1.05f + 10));
        img[idx + 2] = clamp((int)(b * 0.85f));
    }
}

// Remove orange color cast
static void remove_film_color_cast(unsigned char *img, int width, int height, int channels) {
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;
        unsigned char r = img[idx];
        unsigned char g = img[idx + 1];
        unsigned char b = img[idx + 2];

        img[idx] = clamp((int)((r - 20) / 1.15f));
        img[idx + 1] = clamp((int)((g - 10) / 1.05f));
        img[idx + 2] = clamp((int)(b / 0.85f));
    }
}

// Add film grain
static void apply_grain(unsigned char *img, int width, int height, int channels, int intensity) {
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;
        int grain = random_grain(intensity);
        img[idx] = clamp(img[idx] + grain);
        img[idx + 1] = clamp(img[idx + 1] + grain);
        img[idx + 2] = clamp(img[idx + 2] + grain);
    }
}

// Draw sprocket holes
static void draw_sprocket_holes(unsigned char *img, int width, int height, int channels) {
    int border_height = height / 15;
    int hole_width = width / 25;
    int hole_height = border_height / 2;
    int spacing = width / 12;

    for (int y = 0; y < border_height; y++) {
        for (int x = 0; x < width; x++) {
            int top_idx = (y * width + x) * channels;
            int bottom_idx = ((height - 1 - y) * width + x) * channels;

            img[top_idx] = 220;
            img[top_idx + 1] = 150;
            img[top_idx + 2] = 130;
            img[bottom_idx] = 220;
            img[bottom_idx + 1] = 150;
            img[bottom_idx + 2] = 130;
        }
    }

    for (int hole_num = 0; hole_num < width / spacing; hole_num++) {
        int hole_x = hole_num * spacing + spacing / 4;
        for (int y = border_height / 4; y < border_height / 4 + hole_height; y++) {
            for (int x = hole_x; x < hole_x + hole_width && x < width; x++) {
                int top_idx = (y * width + x) * channels;
                int bottom_idx = ((height - 1 - y) * width + x) * channels;

                img[top_idx] = 240;
                img[top_idx + 1] = 240;
                img[top_idx + 2] = 240;
                img[bottom_idx] = 240;
                img[bottom_idx + 1] = 240;
                img[bottom_idx + 2] = 240;
            }
        }
    }
}

// Remove sprocket holes
static void crop_sprocket_holes(unsigned char *img, int width, int height, int channels) {
    int border_height = height / 15;

    for (int y = 0; y < border_height; y++) {
        for (int x = 0; x < width; x++) {
            int top_idx = (y * width + x) * channels;
            int bottom_idx = ((height - 1 - y) * width + x) * channels;

            img[top_idx] = 0;
            img[top_idx + 1] = 0;
            img[top_idx + 2] = 0;
            img[bottom_idx] = 0;
            img[bottom_idx + 1] = 0;
            img[bottom_idx + 2] = 0;
        }
    }
}

// Main processing function
ImageResult process_image(const unsigned char *input_data, size_t input_size, ProcessMode mode) {
    ImageResult result = {0};

    // Initialize random seed
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }

    // Load image from memory
    int width, height, channels;
    unsigned char *img = stbi_load_from_memory(input_data, input_size, &width, &height, &channels, 0);

    if (img == NULL) {
        result.success = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                "Failed to load image: %s", stbi_failure_reason());
        return result;
    }

    if (channels < 3) {
        result.success = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                "Image must have at least 3 channels (RGB)");
        stbi_image_free(img);
        return result;
    }

    // Apply processing based on mode
    if (mode == MODE_TO_NEGATIVE) {
        apply_negative(img, width, height, channels);
        apply_film_color_cast(img, width, height, channels);
        apply_grain(img, width, height, channels, 12);
        draw_sprocket_holes(img, width, height, channels);
    } else {
        crop_sprocket_holes(img, width, height, channels);
        remove_film_color_cast(img, width, height, channels);
        apply_negative(img, width, height, channels);
    }

    // Prepare result
    result.data = img;
    result.width = width;
    result.height = height;
    result.channels = channels;
    result.success = 1;
    result.error_message[0] = '\0';

    return result;
}

// Free image result
void free_image_result(ImageResult *result) {
    if (result && result->data) {
        stbi_image_free(result->data);
        result->data = NULL;
    }
}

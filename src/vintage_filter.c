/*
 * Film Negative Filter
 *
 * Transforms digital images into film negative appearance with:
 * - Color inversion (negative effect)
 * - Orange/amber color cast (like real film negatives)
 * - Film grain texture
 * - Edge darkening
 * - Film sprocket hole borders (optional)
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

// Clamp value between 0 and 255
unsigned char clamp(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (unsigned char)value;
}

// Generate random number between -range and +range for grain effect
int random_grain(int range) {
    return (rand() % (range * 2 + 1)) - range;
}

// Invert colors to create negative effect
void apply_negative(unsigned char *img, int width, int height, int channels) {
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;

        // Invert each color channel
        img[idx] = 255 - img[idx];         // R
        img[idx + 1] = 255 - img[idx + 1]; // G
        img[idx + 2] = 255 - img[idx + 2]; // B
    }
}

// Apply orange/amber color cast like real film negatives
void apply_film_color_cast(unsigned char *img, int width, int height, int channels) {
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;

        unsigned char r = img[idx];
        unsigned char g = img[idx + 1];
        unsigned char b = img[idx + 2];

        // Film negative has orange/amber base
        // Boost orange and red tones
        int new_r = (int)(r * 1.15f + 20);
        int new_g = (int)(g * 1.05f + 10);
        int new_b = (int)(b * 0.85f);

        img[idx] = clamp(new_r);
        img[idx + 1] = clamp(new_g);
        img[idx + 2] = clamp(new_b);
    }
}

// Remove orange/amber color cast (reverse operation)
void remove_film_color_cast(unsigned char *img, int width, int height, int channels) {
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;

        unsigned char r = img[idx];
        unsigned char g = img[idx + 1];
        unsigned char b = img[idx + 2];

        // Reverse the color cast adjustments
        int new_r = (int)((r - 20) / 1.15f);
        int new_g = (int)((g - 10) / 1.05f);
        int new_b = (int)(b / 0.85f);

        img[idx] = clamp(new_r);
        img[idx + 1] = clamp(new_g);
        img[idx + 2] = clamp(new_b);
    }
}

// Crop sprocket holes from image
void crop_sprocket_holes(unsigned char *img, int width, int height, int channels) {
    int border_height = height / 15;

    // Fill borders with black (or you could crop the image)
    for (int y = 0; y < border_height; y++) {
        for (int x = 0; x < width; x++) {
            int top_idx = (y * width + x) * channels;
            int bottom_idx = ((height - 1 - y) * width + x) * channels;

            // Make borders black/transparent
            img[top_idx] = 0;
            img[top_idx + 1] = 0;
            img[top_idx + 2] = 0;

            img[bottom_idx] = 0;
            img[bottom_idx + 1] = 0;
            img[bottom_idx + 2] = 0;
        }
    }
}

// Apply film grain effect
void apply_grain(unsigned char *img, int width, int height, int channels, int intensity) {
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;

        int grain = random_grain(intensity);

        img[idx] = clamp(img[idx] + grain);
        img[idx + 1] = clamp(img[idx + 1] + grain);
        img[idx + 2] = clamp(img[idx + 2] + grain);
    }
}

// Draw film sprocket holes at top and bottom
void draw_sprocket_holes(unsigned char *img, int width, int height, int channels) {
    int border_height = height / 15;  // Height of film border
    int hole_width = width / 25;
    int hole_height = border_height / 2;
    int spacing = width / 12;

    // Draw top and bottom borders
    for (int y = 0; y < border_height; y++) {
        for (int x = 0; x < width; x++) {
            int top_idx = (y * width + x) * channels;
            int bottom_idx = ((height - 1 - y) * width + x) * channels;

            // Orange/pink film base color
            img[top_idx] = 220;
            img[top_idx + 1] = 150;
            img[top_idx + 2] = 130;

            img[bottom_idx] = 220;
            img[bottom_idx + 1] = 150;
            img[bottom_idx + 2] = 130;
        }
    }

    // Draw sprocket holes
    for (int hole_num = 0; hole_num < width / spacing; hole_num++) {
        int hole_x = hole_num * spacing + spacing / 4;

        for (int y = border_height / 4; y < border_height / 4 + hole_height; y++) {
            for (int x = hole_x; x < hole_x + hole_width && x < width; x++) {
                int top_idx = (y * width + x) * channels;
                int bottom_idx = ((height - 1 - y) * width + x) * channels;

                // White holes
                img[top_idx] = 240;
                img[top_idx + 1] = 240;
                img[top_idx + 2] = 240;

                img[bottom_idx] = 240;
                img[bottom_idx + 1] = 240;
                img[bottom_idx + 2] = 240;
            }
        }
    }

    // Add frame numbers on the border
    // (simplified - just adds some visual elements)
}

// Main film negative filter function
void apply_film_negative_filter(unsigned char *img, int width, int height, int channels) {
    printf("Applying film negative effects...\n");

    printf("  - Inverting colors (negative effect)...\n");
    apply_negative(img, width, height, channels);

    printf("  - Applying film color cast (orange/amber)...\n");
    apply_film_color_cast(img, width, height, channels);

    printf("  - Adding film grain texture...\n");
    apply_grain(img, width, height, channels, 12);

    printf("  - Drawing film sprocket holes...\n");
    draw_sprocket_holes(img, width, height, channels);

    printf("Film negative filter applied successfully!\n");
}

// Reverse film negative back to positive (color) image
void reverse_film_negative(unsigned char *img, int width, int height, int channels) {
    printf("Reversing film negative to positive image...\n");

    printf("  - Removing sprocket hole borders...\n");
    crop_sprocket_holes(img, width, height, channels);

    printf("  - Removing film color cast...\n");
    remove_film_color_cast(img, width, height, channels);

    printf("  - Inverting colors back to positive...\n");
    apply_negative(img, width, height, channels);

    printf("Negative to positive conversion complete!\n");
    printf("Note: Film grain cannot be fully removed as it's random.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Film Negative Filter\n");
        printf("====================\n");
        printf("Usage: %s <input_image> <output_image> [mode]\n", argv[0]);
        printf("\nModes:\n");
        printf("  -n  : Convert to negative (default)\n");
        printf("  -r  : Reverse negative back to positive\n");
        printf("\nExamples:\n");
        printf("  %s photo.jpg film_negative.jpg      # Convert to negative\n", argv[0]);
        printf("  %s negative.jpg restored.jpg -r     # Reverse to positive\n", argv[0]);
        printf("\nSupported formats:\n");
        printf("  Input:  JPG, PNG, BMP, TGA\n");
        printf("  Output: JPG, PNG, BMP, TGA\n");
        return 1;
    }

    char *input_file = argv[1];
    char *output_file = argv[2];
    char *mode = (argc == 4) ? argv[3] : "-n";  // Default to negative mode

    // Initialize random seed for grain effect
    srand(time(NULL));

    printf("\n=== Film Negative Filter ===\n\n");
    printf("Loading image: %s\n", input_file);

    int width, height, channels;
    unsigned char *img = stbi_load(input_file, &width, &height, &channels, 0);

    if (img == NULL) {
        printf("Error: Could not load image '%s'\n", input_file);
        printf("Reason: %s\n", stbi_failure_reason());
        return 1;
    }

    printf("Image loaded successfully!\n");
    printf("  Dimensions: %dx%d\n", width, height);
    printf("  Channels: %d\n\n", channels);

    if (channels < 3) {
        printf("Error: Image must have at least 3 channels (RGB)\n");
        stbi_image_free(img);
        return 1;
    }

    // Apply filter based on mode
    if (strcmp(mode, "-r") == 0) {
        // Reverse negative to positive
        reverse_film_negative(img, width, height, channels);
    } else {
        // Default: Apply negative filter
        apply_film_negative_filter(img, width, height, channels);
    }

    // Determine output format from extension
    printf("\nSaving image: %s\n", output_file);

    int result = 0;
    char *ext = strrchr(output_file, '.');

    if (ext != NULL) {
        if (strcmp(ext, ".png") == 0 || strcmp(ext, ".PNG") == 0) {
            result = stbi_write_png(output_file, width, height, channels, img, width * channels);
        } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".JPG") == 0 ||
                   strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".JPEG") == 0) {
            result = stbi_write_jpg(output_file, width, height, channels, img, 90);
        } else if (strcmp(ext, ".bmp") == 0 || strcmp(ext, ".BMP") == 0) {
            result = stbi_write_bmp(output_file, width, height, channels, img);
        } else if (strcmp(ext, ".tga") == 0 || strcmp(ext, ".TGA") == 0) {
            result = stbi_write_tga(output_file, width, height, channels, img);
        } else {
            printf("Warning: Unknown format, defaulting to PNG\n");
            result = stbi_write_png(output_file, width, height, channels, img, width * channels);
        }
    } else {
        result = stbi_write_png(output_file, width, height, channels, img, width * channels);
    }

    if (result) {
        printf("Image saved successfully!\n");
        printf("\n=== Processing Complete ===\n\n");
    } else {
        printf("Error: Could not save image to '%s'\n", output_file);
    }

    stbi_image_free(img);

    return result ? 0 : 1;
}

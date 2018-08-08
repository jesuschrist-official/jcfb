#include <stdint.h>
#include <stdio.h>

#include "jcfb/bitmap-io.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


int bitmap_load(bitmap_t* bmp, const char* path) {
    int w, h, n;
    pixel_t* data = (pixel_t*)stbi_load(path, &w, &h, &n, 4);
    if (!data) {
        fprintf(stderr, "Couldn't read file '%s'\n", path);
        goto error;
    }
    if (bitmap_init(bmp, w, h) != 0) {
        goto error;
    }

    pixel_t* dst = bmp->mem;
    pixel_t* src = data;
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            *dst = pixel_conv(PIXFMT_ABGR32, bmp->fmt, *src);
            // Handling alpha
            if ((*src & 0x000000ff) == 0x000000ff) {
                *dst = pixel_to(bmp->fmt, 0x00ff00ff);
            }
            dst++;
            src++;
        }
    }

    stbi_image_free(data);
    return 0;

  error:
    if (data) stbi_image_free(data);
    return 1;
}

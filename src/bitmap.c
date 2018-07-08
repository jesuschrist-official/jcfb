#include <string.h>

#include "jcfb/bitmap.h"


int bitmap_init(bitmap_t* bmp, const pixfmt_t* fmt, int w, int h) {
    *bmp = (bitmap_t){
        .w = w,
        .h = h,
        .fmt = *fmt,
        .mem8 = calloc((w * h * fmt->bpp) / 8, 1),
        .flags = BITMAP_FLAG_MEM_OWNER,
    };
    if (!bmp->mem8) {
        return -1;
    }
    return 0;
}


int bitmap_init_from_memory(bitmap_t* bmp, const pixfmt_t* fmt, int w, int h,
                            void* mem)
{
    *bmp = (bitmap_t){
        .w = w,
        .h = h,
        .fmt = *fmt,
        .mem8 = mem,
        .flags = 0,
    };
    return 0;
}


void bitmap_wipe(bitmap_t* bmp) {
    if ((bmp->flags & BITMAP_FLAG_MEM_OWNER) && bmp->mem8) {
        free(bmp->mem8);
        bmp->mem8 = NULL;
    }
}


size_t bitmap_memsize(const bitmap_t* bmp) {
    return (bmp->fmt.bpp * bmp->w * bmp->h) / 8;
}


void bitmap_put_pixel(bitmap_t* bmp, int x, int y, pixel_t color) {
    uint8_t* dst = bmp->mem8 + ((y * bmp->w + x) * bmp->fmt.bpp) / 8;
    pixel_t dst_color = pixel(&bmp->fmt, color);
    memcpy(dst, &dst_color, bmp->fmt.bpp / 8);
}


void bitmap_clear(bitmap_t* bmp, pixel_t color) {
    for (int y = 0; y < bmp->h; y++) {
        for (int x = 0; x < bmp->w; x++) {
            bitmap_put_pixel(bmp, x, y, color);
        }
    }
}

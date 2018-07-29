#include <string.h>

#include "jcfb/bitmap.h"


int bitmap_init(bitmap_t* bmp, int w, int h) {
    *bmp = (bitmap_t){
        .w = w,
        .h = h,
        .fmt = PIXFMT_FB,
        .mem = calloc(w * h, sizeof(pixel_t)),
        .flags = BITMAP_FLAG_MEM_OWNER,
    };
    if (!bmp->mem) {
        return -1;
    }
    return 0;
}


int bitmap_init_from_memory(bitmap_t* bmp, int w, int h, void* mem)
{
    *bmp = (bitmap_t){
        .w = w,
        .h = h,
        .fmt = PIXFMT_FB,
        .mem = mem,
        .flags = 0,
    };
    return 0;
}


int bitmap_init_ex(bitmap_t* bmp, pixfmt_id_t fmt, int w, int h) {
    *bmp = (bitmap_t){
        .w = w,
        .h = h,
        .fmt = fmt,
        .mem = calloc(w * h, sizeof(pixel_t)),
        .flags = BITMAP_FLAG_MEM_OWNER,
    };
    if (!bmp->mem) {
        return -1;
    }
    return 0;
}


void bitmap_wipe(bitmap_t* bmp) {
    if ((bmp->flags & BITMAP_FLAG_MEM_OWNER) && bmp->mem) {
        free(bmp->mem);
        bmp->mem = NULL;
    }
}


size_t bitmap_memsize(const bitmap_t* bmp) {
    return bmp->w * bmp->h * sizeof(pixel_t);
}


void bitmap_put_pixel(bitmap_t* bmp, int x, int y, pixel_t color) {
    bmp->mem[y * bmp->w + x] = pixel_to(bmp->fmt, color);
}


void bitmap_clear(bitmap_t* bmp, pixel_t color) {
    color = pixel_to(bmp->fmt, color);
    for (int y = 0; y < bmp->h; y++) {
        for (int x = 0; x < bmp->w; x++) {
            bmp->mem[y * bmp->w + x] = color;
        }
    }
}


static void _fast_blit(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    for (int sy = 0; sy < src->h; sy++) {
        memcpy(dst->mem + (y + sy) * dst->w + x,
               src->mem + sy * src->w,
               src->w * sizeof(pixel_t));
    }
}


static void _slow_blit(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    for (int sy = 0; sy < src->h; sy++) {
        pixel_t* dest_addr = dst->mem + (y + sy) * dst->w + x;
        memcpy(dest_addr,
               src->mem + sy * src->w,
               src->w * sizeof(pixel_t));
        for (int sx = 0; sx < src->w; sx++) {
            *(dest_addr + sx) = pixel_conv(src->fmt, dst->fmt,
                                           *(dest_addr + sx));
        }
    }
}


void bitmap_blit(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    if (dst->fmt == src->fmt) {
        _fast_blit(dst, src, x, y);
    } else {
        _slow_blit(dst, src, x, y);
    }
}

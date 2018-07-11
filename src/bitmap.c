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


static size_t _get_pixel_size(const bitmap_t* bmp) {
    return bmp->fmt.bpp / 8;
}


static size_t _get_line_size(const bitmap_t* bmp) {
    return bmp->w * _get_pixel_size(bmp);
}


static const void* _get_pixel_addr(const bitmap_t* bmp, int x, int y) {
    return bmp->mem + (y * _get_line_size(bmp) + x * _get_pixel_size(bmp));
}


static void _fast_blit(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    for (int sy = 0; sy < src->h; sy++) {
        const void* src_addr = _get_pixel_addr(src, 0, sy);
        void* dst_addr = (void*)_get_pixel_addr(dst, x, sy + y);
        memcpy(dst_addr, src_addr, _get_line_size(src));
    }
}


static void _slow_blit(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    for (int sy = 0; sy < src->h; sy++) {
        for (int sx = 0; sx < src->w; sx++) {
            int dx = x + sx;
            int dy = y + sy;
            uint8_t* src_addr = src->mem8
                              + (sy * src->w + sx) * (src->fmt.bpp / 8);
            uint8_t* dst_addr = dst->mem8
                              + (dy * dst->w + dx) * (dst->fmt.bpp / 8);
            pixel_t src_color = 0;
            memcpy(&src_color, src_addr, src->fmt.bpp / 8);
            pixel_t dst_color = pixel_conv(&src->fmt, &dst->fmt, src_color);
            memcpy(dst_addr, &dst_color, dst->fmt.bpp / 8);
        }
    }
}


void bitmap_blit(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    if (dst->fmt.id == src->fmt.id) {
        _fast_blit(dst, src, x, y);
    } else {
        _slow_blit(dst, src, x, y);
    }
}

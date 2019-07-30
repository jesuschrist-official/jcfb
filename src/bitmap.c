/*
 * JCFB bitmap implementations
 *
 * XXX Maybe we could use an additional member `stride` to `bitmap_t`,
 *     indicating the number of pixels between two rows.
 *     This could be used to do regions of bitmaps without memory copy,
 *     allowing for exemple to do clipping.
 */
#include <stdbool.h>
#include <string.h>


#include "jcfb/bitmap.h"
#include "jcfb/util.h"


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


pixel_t* bitmap_pixel_addr(bitmap_t* bmp, int x, int y) {
    return &bmp->mem[y * bmp->w + x];
}


pixel_t bitmap_pixel(const bitmap_t* bmp, int x, int y) {
    return bmp->mem[y * bmp->w + x];
}


size_t bitmap_memsize(const bitmap_t* bmp) {
    return bmp->w * bmp->h * sizeof(pixel_t);
}


static pixel_t _pixel_blend_add(pixel_t dst, pixel_t src) {
    int dr, dg, db;
    int sr, sg, sb;
    read_rgb(dst, &dr, &dg, &db);
    read_rgb(src, &sr, &sg, &sb);
    dr = min(255, dr + sr);
    dg = min(255, dg + sg);
    db = min(255, db + sb);
    return rgb(dr, dg, db);
}


void bitmap_put_pixel(bitmap_t* bmp, int x, int y, pixel_t color) {
    if (x < 0 || x >= bmp->w || y < 0 || y >= bmp->h) {
        return;
    }
    bmp->mem[y * bmp->w + x] = color;
}


void bitmap_put_pixel_blend_add(bitmap_t* bmp, int x, int y, pixel_t color) {
    if (x < 0 || x >= bmp->w || y < 0 || y >= bmp->h) {
        return;
    }
    bmp->mem[y * bmp->w + x] = _pixel_blend_add(
        bmp->mem[y * bmp->w + x], color
    );
}


void bitmap_clear(bitmap_t* bmp, pixel_t color) {
    for (int y = 0; y < bmp->h; y++) {
        for (int x = 0; x < bmp->w; x++) {
            bmp->mem[y * bmp->w + x] = color;
        }
    }
}


bool bitmap_is_in(const bitmap_t* bmp, int x, int y) {
    return !(x < 0 || x >= bmp->w || y < 0 || y >= bmp->h);
}


static void _convert_row(bitmap_t* dst, int x, int dy,
                         pixfmt_id_t pixfmt)
{
    int dx = max(0, x);
    for (; dx < dst->w; dx++) {
        pixel_t old_color = dst->mem[dy * dst->w + dx];
        pixel_t new_color = pixel_conv(pixfmt, dst->fmt, old_color);
        dst->mem[dy * dst->w + dx] = new_color;
    }
}


#define BLIT_PIXEL_FUNC(_dst, _src) _dst = _src
#define BLIT_FUNC_SUFFIX
#include "bitmap-blit.inc.c"


#define BLIT_PIXEL_FUNC(_dst, _src) _dst = _pixel_blend_add(_dst, _src)
#define BLIT_FUNC_SUFFIX _blend_add
#include "bitmap-blit.inc.c"


#define BLIT_PIXEL_FUNC(_dst, _src) \
    if (_src != get_mask_color()) { \
        _dst = _src; \
    }
#define BLIT_FUNC_SUFFIX _masked
#include "bitmap-blit.inc.c"

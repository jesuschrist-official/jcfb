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



// In the following blit functions,
// x, y are the coordinates requested by the user,
// dx, dy are the current final coordinates on the `dst` bitmap and
// sx, sy are the current final coordinates on the `src` bitmap.
static void _blit_row(bitmap_t* dst, const bitmap_t* src,
                      int x, int dy, int sy)
{
    int dx = max(0, x);
    int sx = max(0, -x);
    int max_size = min(src->w - sx, dst->w - dx);
    if (max_size < 0) {
        return;
    }
    pixel_t* dest_addr = dst->mem + dy * dst->w + dx;
    const pixel_t* src_addr = src->mem + sy * src->w + sx;
    memcpy(dest_addr, src_addr, max_size * sizeof(pixel_t));
}


static void _fast_blit(bitmap_t* dst, const bitmap_t* src,
                       int x, int y)
{
    // If `y` is offscreen, we start to copy `src` from the `-y` row to
    // `dst` on the first row.
    int dy = max(0, y);
    int sy = max(0, -y);
    for (; dy < dst->h && sy < src->h; dy++, sy++) {
        _blit_row(dst, src, x, dy, sy);
    }
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


static void _slow_blit(bitmap_t* dst, const bitmap_t* src,
                       int x, int y)
{
    int dy = max(0, y);
    int sy = max(0, -dy);
    for (; dy < dst->h && sy < src->h; dy++, sy++) {
        _blit_row(dst, src, x, dy, sy);
        _convert_row(dst, x, dy, src->fmt);
    }
}


void bitmap_blit(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    if (dst->fmt == src->fmt) {
        _fast_blit(dst, src, x, y);
    } else {
        _slow_blit(dst, src, x, y);
    }
}


static void _blit_scaled_row(bitmap_t* dst, const bitmap_t* src,
                             int x, int y, int w, int sy)
{
    int dx = max(0, x);
    float sx = max(0, -x);
    float xratio = src->w / (float)w;
    for (; dx < min(x + w, dst->w) && sx * xratio < src->w; dx++, sx++)
    {
        size_t src_off = sy * src->w + sx * xratio;
        dst->mem[y * dst->w + dx] = src->mem[src_off];
    }
}


void bitmap_scaled_blit(bitmap_t* dst, const bitmap_t* src,
                        int x, int y, int w, int h)
{
    int dy = max(0, y);
    float sy = max(0, -y);
    float yratio = src->h / (float)h;
    for (; dy < min(y + h, dst->h) && sy * yratio < src->h; dy++, sy++)
    {
        _blit_scaled_row(dst, src, x, dy, w, sy * yratio);
    }
}


static void _blit_scaled_region_row(bitmap_t* dst, const bitmap_t* src,
                                    int dst_x, int dst_y, int dst_w,
                                    int src_x, int src_y,
                                    int src_w)
{
    int dx = max(0, dst_x);
    int sx = max(0, -dst_x);

    int dx_max = min(dst_x + dst_w, dst->w);
    int sx_max = min(src_x + src_w, src->w);

    float xratio = src_w / (float)dst_w;

    for (; dx < dx_max && src_x + sx * xratio < sx_max; dx++, sx++) {
        size_t dst_off = dst_y * dst->w + dx;
        size_t src_off = (size_t)(src_y * src->w + src_x + sx * xratio);
        dst->mem[dst_off] = src->mem[src_off];
    }
}


void bitmap_scaled_region_blit(bitmap_t* dst, const bitmap_t* src,
                               int src_x, int src_y, int src_w,
                               int src_h,
                               int dst_x, int dst_y, int dst_w,
                               int dst_h)
{
    int dy = max(0, dst_y);
    float sy = max(0, -dst_y);

    int dy_max = min(dst_y + dst_h, dst->h);
    float sy_max = min(src_y + src_h, src->h);

    float yratio = src_h / (float)dst_h;

    for (; dy < dy_max && src_y + sy * yratio < sy_max; dy++, sy++) {
        _blit_scaled_region_row(dst, src,
                                dst_x, dy, dst_w,
                                src_x, src_y + sy * yratio, src_w);
    }
}


static void _bitmap_masked_blit_row(bitmap_t* dst, const bitmap_t* src,
                                    int dx, int dy, int sy)
{
    pixel_t mask = pixel_to(src->fmt, 0x00ff00ff);

    // If `dx` is offscreen, we start to copy the row from '-dx'.
    int sx = max(0, -dx);
    dx = max(0, dx);
    for (; dx < dst->w && sx < src->w; dx++, sx++) {
        pixel_t p = src->mem[sy * src->w + sx];
        if (p != mask) {
            dst->mem[dy * dst->w + dx] = p;
        }
    }
}


void bitmap_masked_blit(bitmap_t* dst, const bitmap_t* src,
                        int x, int y)
{
    int sy = max(0, -y);
    int dy = max(0, y);
    for (; dy < dst->h && sy < src->h; dy++, sy++) {
        _bitmap_masked_blit_row(dst, src, x, dy, sy);
    }
}


bool bitmap_is_in(const bitmap_t* bmp, int x, int y) {
    return !(x < 0 || x >= bmp->w || y < 0 || y >= bmp->h);
}

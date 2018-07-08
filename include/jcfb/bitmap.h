/*
 * Bitmap module
 *
 * Load, create and manipulate bitmaps.
 */
#ifndef _jcfb_bitmap_h_
#define _jcfb_bitmap_h_

#include "jcfb/pixel.h"


/*
 * Bitmap flags
 */
enum {
    /*
     * The bitmap doesn't own its memory
     */
    BITMAP_FLAG_MEM_OWNER = 0x01,
};


/*
 * Bitmap structure
 */
typedef struct bitmap {
    size_t w, h;
    pixfmt_t fmt;
    union {
        uint8_t* mem8;
        uint16_t* mem16;
        uint32_t* mem32;
    };
    uint32_t flags;
} bitmap_t;


/*
 * Initialize a bitmap having the given dimensions and pixel format.
 */
int bitmap_init(bitmap_t* bmp, const pixfmt_t* fmt, int w, int h);


/*
 * Initialize a bitmap having the given dimensions and pixel format pointing
 * to a foreign memory area.
 */
int bitmap_init_from_memory(bitmap_t* bmp, const pixfmt_t* fmt, int w, int h,
                            void* mem);


/*
 * Wipe the bitmap's memory.
 */
void bitmap_wipe(bitmap_t* bmp);


/*
 * Returns the size of the bitmap allocated space in bytes.
 */
size_t bitmap_memsize(const bitmap_t* bmp);


/*
 * Put a RGBA32 pixel at the given coordinates of the bitmap.
 * Notates that this function is slow due to its genericity.
 */
void bitmap_put_pixel(bitmap_t* bmp, int x, int y, pixel_t color);


/*
 * Clear every pixels of the given bitmap with the given RGBA32 color.
 */
void bitmap_clear(bitmap_t* bmp, pixel_t color);


#endif

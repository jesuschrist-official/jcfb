/*
 * JCFB primitive module - Draw primitive shapes
 *
 * For every drawing function, the color must be encoded using the bitmap
 * pixel format.
 */
#ifndef _jcfb_primitive_h_
#define _jcfb_primitive_h_


#include "jcfb/bitmap.h"


void draw_hline(bitmap_t* bmp, pixel_t color, int x1, int x2, int y);


void draw_vline(bitmap_t* bmp, pixel_t color, int x, int y1, int y2);


void draw_line(bitmap_t* bmp, pixel_t color, int x1, int y1, int x2, int y2);


void draw_rect(bitmap_t* bmp, pixel_t color, int x1, int y1, int x2, int y2);


#endif

/*
 * JCFB primitive module - Draw primitive shapes
 *
 * For every drawing function, the color must be encoded using the
 * target bitmap's pixel format.
 */
#ifndef _jcfb_primitive_h_
#define _jcfb_primitive_h_


#include "jcfb/bitmap.h"


/*
 * Draw a horizontal line between points (x1, y) and (x2, y).
 */
void draw_hline(bitmap_t* bmp, pixel_t color, int x1, int x2, int y);


/*
 * Draw a vertical line between points (x, y1) and (x, y2).
 */
void draw_vline(bitmap_t* bmp, pixel_t color, int x, int y1, int y2);


/*
 * Draw a line between points (x1, y1) and (x2, y2).
 */
void draw_line(bitmap_t* bmp, pixel_t color,
               int x1, int y1, int x2, int y2);


/*
 * Draw a rectangle where (x1, y1) is the top-left corner, and (x2, y2)
 * the bottom-right.
 */
void draw_rect(bitmap_t* bmp, pixel_t color,
               int x1, int y1, int x2, int y2);


#endif

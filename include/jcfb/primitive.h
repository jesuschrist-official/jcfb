/*
 * JCFB primitive module - Draw primitive shapes
 *
 * For every drawing function, the color must be encoded using the
 * target bitmap's pixel format.
 */
#ifndef _jcfb_primitive_h_
#define _jcfb_primitive_h_


#include "jcfb/bitmap.h"


/* Regular functions ------------------------------------------------------- */
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


/*
 * Fill a rectangle where (x1, y1) is the top-left corner, and (x2, y2)
 * the bottom-right.
 */
void fill_rect(bitmap_t* bmp, pixel_t color,
               int x1, int y1, int x2, int y2);


/*
 * Draw a circle of radius `r` and center (`x`, `y`) using color
 * `color`.
 */
void draw_circle(bitmap_t* bmp, pixel_t color, int x, int y, int r);


/*
 * Fill a circle of radius `r` and center (`x`, `y`) using color
 * `color`.
 */
void fill_circle(bitmap_t* bmp, pixel_t color, int x, int y, int r);


/*
 * Draw a dashed horizontal line.
 *
 * `dash_length` is the length of a dash (and of a gape).
 * `dash_start` can be used to give the length of the first dash, for
 * exemple in case of line continuation. If it satisfies the formula
 * `(line_length + dash_start) % (2 * dash_length) < dash_length, it
 * means that the first color will be used. Otherwise, the second
 * color will be used.
 * `direction` inverse the direction in which dashes will be drawn.
 * `stroke` is used to specify a line width. line is centered on the
 * given coordinates.
 */
void draw_dashed_hline(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
                       int x1, int x2, int y,
                       int dash_start, int dash_length, int direction,
                       int stroke);


/*
 * Draw a dashed vertical line.
 */
void draw_dashed_vline(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
                       int x, int y1, int y2,
                       int dash_start, int dash_length, int direction,
                       int stroke);


/*
 * Draw a dashed rect.
 */
void draw_dashed_rect(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
                      int x1, int y1, int x2, int y2,
                      int dash_start, int dash_length, int stroke);


/* Additive blending functions --------------------------------------------- */
void draw_hline_blend_add(bitmap_t* bmp, pixel_t color, int x1, int x2, int y);


void draw_vline_blend_add(bitmap_t* bmp, pixel_t color,
                          int x, int y1, int y2);


void draw_line_blend_add(bitmap_t* bmp, pixel_t color,
                         int x1, int y1, int x2, int y2);


void draw_rect_blend_add(bitmap_t* bmp, pixel_t color,
                         int x1, int y1, int x2, int y2);


void fill_rect_blend_add(bitmap_t* bmp, pixel_t color,
                         int x1, int y1, int x2, int y2);


void draw_circle_blend_add(bitmap_t* bmp, pixel_t color, int x, int y, int r);


void fill_circle_blend_add(bitmap_t* bmp, pixel_t color, int x, int y, int r);


void draw_dashed_hline_blend_add(bitmap_t* bmp,
                                 pixel_t color_a, pixel_t color_b,
                                 int x1, int x2, int y,
                                 int dash_start, int dash_length,
                                 int direction,
                                 int stroke);


void draw_dashed_vline_blend_add(bitmap_t* bmp,
                                 pixel_t color_a, pixel_t color_b,
                                 int x, int y1, int y2,
                                 int dash_start, int dash_length,
                                 int direction,
                                 int stroke);


void draw_dashed_rect_blend_add(bitmap_t* bmp,
                                pixel_t color_a, pixel_t color_b,
                                int x1, int y1, int x2, int y2,
                                int dash_start, int dash_length, int stroke);


/* ------------------------------------------------------------------------- */


#endif

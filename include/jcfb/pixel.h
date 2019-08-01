/*
 * Pixel manipulation
 *
 * This module provides tools to do color conversion between various
 * pixel formats.
 *
 * JCFB achieves color conversion in a generic way. This is cool, but
 * it has a hudge conversion cost (on my computer in release mode,
 * blitting a bitmap without color conversion is seventy times faster
 * than with color conversion).
 *
 * To avoid the need to do many color conversions, every data created
 * by JCFB will be encoded using the framebuffer internal pixel format.
 *
 * At any time in the code, and once JCFB has been started, using the
 * `pixel()` function will convert a RGBA32 color to the framebuffer
 * format.
 */
#ifndef _jcfb_pixel_h_
#define _jcfb_pixel_h_


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/*
 * Color components
 */
typedef enum {
    COMP_RED,
    COMP_GREEN,
    COMP_BLUE,
    COMP_ALPHA,
    COMP_MAX,
} component_t;


/*
 * Most common pixel formats enumeration
 */
typedef enum {
    PIXFMT_FB = 0,
    PIXFMT_RGB16,
    PIXFMT_RGB24,
    PIXFMT_BGR24,
    PIXFMT_RGBA32,
    PIXFMT_ARGB32,
    PIXFMT_ABGR32
} pixfmt_id_t;


/*
 * Pixel format
 *
 * This structure is used to define the format a pixel has.
 * A pixfmt_t allows to define what size & offset has a color
 * component.
 * This structure can be used to compute pixel conversions.
 */
typedef struct pixfmt {
    size_t bpp;               /* Bits per pixel */
    uint32_t offs[COMP_MAX];  /* Components offset */
    uint32_t sizes[COMP_MAX]; /* Components length */
    // XXX Maybe we'll need Big/Little Endian information ?
} pixfmt_t;


/*
 * Returns the pixfmt structure of the given type.
 */
pixfmt_t pixfmt_get(pixfmt_id_t which);


/*
 * Set the pixfmt used by the framebuffer
 * XXX Should put this function in a private context ?
 */
void pixfmt_set_fb(const pixfmt_t* fmt);


/*
 * Pixel
 *
 * Simple scalar type to represent a pixel. Pixel value depends of its
 * format.
 */
typedef uint32_t pixel_t;


/*
 * Convert a RGBA32 pixel value to the framebuffer format.
 */
pixel_t pixel(pixel_t rgba32);


/*
 * Convert a RGBA32 pixel value to the given format.
 */
pixel_t pixel_to(pixfmt_id_t fmt, pixel_t rgba32);


/*
 * Convert a pixel from any format to a pixel of any format.
 */
pixel_t pixel_conv(pixfmt_id_t in_fmt, pixfmt_id_t out_fmt, pixel_t p);


/*
 * Create a pixel in the framebuffer format using the given RGB
 * components.
 */
pixel_t rgb(int r, int g, int b);


/*
 * Read the RGB component of the given framebuffer-formated pixel.
 */
void read_rgb(pixel_t p, int* r, int* g, int* b);


/*
 * Query the alpha component of the given framebuffer-formated pixel.
 */
int get_alpha(pixel_t p);


/*
 * Returns true if the given pixel format has an alpha component.
 */
bool has_alpha(pixfmt_id_t fmt);


/*
 * Returns the framebuffer mask color.
 */
pixel_t get_mask_color();


/*
 * Returns the addition of `dst` on `src`
 */
pixel_t pixel_blend_add(pixel_t src, pixel_t dst);


#endif

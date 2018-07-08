/*
 * Pixel manipulation.
 *
 * This module provides tools to do color conversion between various pixel
 * formats.
 */
#ifndef _jcfb_pixel_h_
#define _jcfb_pixel_h_

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
 * Pixel format
 *
 * This structure is used to define the format a pixel has.
 * A pixfmt_t allows to define what size & offset has a color component.
 * This structure can be used to compute pixel conversions.
 */
typedef struct pixfmt {
    size_t bpp;               /* Bits per pixel */
    uint32_t offs[COMP_MAX];  /* Components offset */
    uint32_t sizes[COMP_MAX]; /* Components length */
    // XXX Maybe we'll need Big/Little Endian information
} pixfmt_t;


/*
 * Most common pixel formats enumeration
 */
typedef enum {
    PIXFMT_RGB16,
    PIXFMT_RGB24,
    PIXFMT_RGBA32,
    PIXFMT_ARGB32
} pixfmt_enum_t;


/*
 * Returns the pixfmt structure of the given type.
 */
pixfmt_t pixfmt_get(pixfmt_enum_t which);


/*
 * Pixel
 *
 * Simple scalar type to represents a pixel. Pixel value depends of its
 * format.
 */
typedef uint32_t pixel_t;


/*
 * Convert a RGBA32 pixel value with the given pixel format.
 */
pixel_t pixel(const pixfmt_t* fmt, pixel_t rgba32);


/*
 * Convert a pixel from any format to a pixel of any format.
 */
pixel_t pixel_conv(const pixfmt_t* in_fmt, const pixfmt_t* out_fmt, pixel_t p);


#endif

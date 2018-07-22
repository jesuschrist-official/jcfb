/*
 * JCFB - The Jesus Christ framebuffer manipulation library
 */
#ifndef _jcfb_h_
#define _jcfb_h_


#include "jcfb/pixel.h"
#include "jcfb/bitmap.h"
#include "jcfb/bitmap-io.h"


/*
 * Initialize the framebuffer.
 */
int jcfb_start();


/*
 * Shutdown the framebuffer.
 */
void jcfb_stop();


/*
 * Clear the framebuffer screen to black.
 */
void jcfb_clear();


/*
 * Get a bitmap of the dimension of the framebuffer, having its pixel
 * format.
 * Caller has the responsibility to free returned address and to wipe its
 * content using `bitmap_wipe`.
 */
bitmap_t* jcfb_get_bitmap();


/*
 * Refresh the screen with the given bitmap.
 * Update keyboard status.
 */
void jcfb_refresh(bitmap_t* bmp);


/*
 * Get the next available key press. Returns -1 if none is present.
 */
int jcfb_next_key();


/*
 * Get the framebuffer width.
 */
int jcfb_width();


/*
 * Get the framebuffer height.
 */
int jcfb_height();


#endif

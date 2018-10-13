/*
 * JCFB - The Jesus Christ framebuffer manipulation library
 */
#ifndef _jcfb_h_
#define _jcfb_h_


#include <stdbool.h>


#include "jcfb/pixel.h"
#include "jcfb/bitmap.h"
#include "jcfb/bitmap-io.h"
#include "jcfb/primitive.h"
#include "jcfb/ttf.h"
#include "jcfb/keyboard.h"
#include "jcfb/mouse.h"
#include "jcfb/util.h"


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
 * Initialize `bitmap` as a bitmap of the framebuffer dimensions &
 * pixel format.
 * Caller has the responsibility to wipe the initialized bitmap.
 */
int jcfb_get_bitmap(bitmap_t* bitmap);


/*
 * Refresh the screen with the given bitmap. This bitmap need to have
 * the framebuffer dimensions and pxiel format. Use `jcfb_get_bitmap()`
 * to retrieve such bitmap.
 * Update keyboard & mouse state.
 */
void jcfb_refresh(bitmap_t* bmp);


/*
 * Get the framebuffer width.
 */
int jcfb_width();


/*
 * Get the framebuffer height.
 */
int jcfb_height();


#endif

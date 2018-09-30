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
 * Get a bitmap of the dimension of the framebuffer, having its pixel
 * format.
 * Caller has the responsibility to wipe hte returned bitmap.
 */
int jcfb_get_bitmap(bitmap_t* bitmap);


/*
 * Refresh the screen with the given bitmap.
 * Update keyboard status.
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

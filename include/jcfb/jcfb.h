/*
 * JCFB - The Jesus Christ framebuffer manipulation library
 */
#ifndef _jcfb_h_
#define _jcfb_h_


#include "jcfb/pixel.h"
#include "jcfb/bitmap.h"
#include "jcfb/bitmap-loading.h"


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
 * Get the framebuffer bitmap address.
 */
bitmap_t* jcfb_get_bitmap();


#endif

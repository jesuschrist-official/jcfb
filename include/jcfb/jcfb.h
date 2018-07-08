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
 * This bitmap should be pointing on a backuffer, thus, you're drawing
 * procedure shouldn't be visible until `jcfb_refresh` is called.
 */
bitmap_t* jcfb_get_bitmap();


/*
 * Refresh the screen with the backbufferr.
 */
void jcfb_refresh();


#endif

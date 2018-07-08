/*
 * JCFB - The Jesus Christ framebuffer manipulation library
 */
#ifndef _jcfb_h_
#define _jcfb_h_


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


#endif

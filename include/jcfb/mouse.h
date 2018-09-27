/*
 * JCFB mouse module.
 *
 * The mouse coordinates always range in [0, MOUSE_MAX_RANGE[.
 *
 * To query at a given time the mouse position, use `mouse_x()` and
 * `mouse_y()`.
 *
 * To know which buttons has been pressed, use the polling function
 * `poll_mouse` while there is something to poll. Every mouse events
 * occured between two calls of `mouse_update` will be reported in the
 * chronological order.
 *
 * Every call to `update_mouse()` clear the previous events queue.
 * Every non-polled event will be lost.
 */
#ifndef _jcfb_mouse_h_
#define _jcfb_mouse_h_


#include <stdbool.h>


#define MOUSE_MAX_RANGE   4096


/*
 * Initialize the mouse.
 * Returns -1 on error.
 */
int init_mouse();


/*
 * Stop the mouse.
 */
void stop_mouse();


/*
 * Update the mouse.
 */
int update_mouse();


/*
 * Get mouse x-axis coordinate.
 */
int mouse_x();


/*
 * Get mouse y-axis coordinate.
 */
int mouse_y();


/*
 * Get mouse x-axis speed.
 */
int mouse_x_speed();


/*
 * Get mouse y-axis speed.
 */
int mouse_y_speed();


/*
 * Mouse buttons.
 */
typedef enum {
    MOUSEBTN_LEFT,
    MOUSEBTN_RIGHT,
    MOUSEBTN_MIDDLE,
} mousebtn_t;


/*
 * Mouse event.
 */
typedef struct mouseevt {
    mousebtn_t button;
    bool released; /* True if the button is released false otherwise */
    int x, y;      /* Pointer's x and y coordinates. */
    int dx, dy;    /* Pointer's x and y speed. */
} mouseevt_t;


/*
 * Poll next mouse event. See above.
 * Returns 0 if there is no more mouse events.
 */
int poll_mouse(mouseevt_t* evt);


#endif

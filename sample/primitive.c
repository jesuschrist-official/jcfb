/*
 * Print an image on the screen.
 */
#include <stdio.h>
#include <unistd.h>
#include "jcfb/jcfb.h"


int main(int argc, char** argv) {
    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    bitmap_t buffer;
    jcfb_get_bitmap(&buffer);

    int exit = 0;
    while (!exit) {
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        bitmap_clear(&buffer, 0x00000000);
        fill_circle(&buffer, pixel(0x000000ff), 50, 50, 15);
        draw_circle(&buffer, pixel(0x000000ff), 100, 50, 15);
        jcfb_refresh(&buffer);
        usleep(1000);
    }

    bitmap_wipe(&buffer);

    jcfb_stop();
    return 0;
}

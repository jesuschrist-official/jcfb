/*
 * Sample
 *
 * This source file is the main sample showing how to use JCFB and which
 * capabilities the library has.
 */
#include <stdio.h>
#include <unistd.h>
#include "jcfb/jcfb.h"


int main(int argc, char** argv) {
    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    bitmap_t tiger;
    if (bitmap_load(&tiger, "tiger.bmp") < 0) {
        return 1;
    }
    while (1) {
        bitmap_clear(jcfb_get_bitmap(), 0x000000ff);
        bitmap_blit(jcfb_get_bitmap(), &tiger, 160, 120);
        jcfb_refresh();
        usleep(1E4);
    }

    bitmap_wipe(&tiger);

    jcfb_stop();
    return 0;
}

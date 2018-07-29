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
    if (bitmap_load(&tiger, "tiger.bmp") != 0) {
        return 1;
    }

    bitmap_t* buffer = jcfb_get_bitmap();
    int exit = 0;
    while (!exit) {
        int key = 0;
        while ((key = jcfb_next_key()) >= 0) {
            printf("%d\n", key);
            if (key == 27) {
                exit = 1;
            }
        }
        bitmap_clear(buffer, 0x00000000);
        bitmap_blit(buffer, &tiger, 160, 120);
        jcfb_refresh(buffer);
        usleep(1000);
    }

    bitmap_wipe(&tiger);
    bitmap_wipe(buffer);
    free(buffer);

    jcfb_stop();
    printf("Gentle quit\n");
    return 0;
}

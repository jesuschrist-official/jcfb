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

    bitmap_t player;
    if (bitmap_load(&player, "data/player.bmp") != 0) {
        return 1;
    }
    int player_x = jcfb_width() / 2 - player.w / 2,
        player_y = jcfb_height() / 2 - player.h / 2;

    bitmap_t* buffer = jcfb_get_bitmap();
    bitmap_clear(buffer, 0x00000000);


    int exit = 0;
    while (!exit) {
        if (jcfb_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        if (jcfb_key_pressed(KEYC_UP)) {
            player_y -= 16;
        }
        if (jcfb_key_pressed(KEYC_DOWN)) {
            player_y += 16;
        }
        if (jcfb_key_pressed(KEYC_LEFT)) {
            player_x -= 16;
        }
        if (jcfb_key_pressed(KEYC_RIGHT)) {
            player_x += 16;
        }
        bitmap_clear(buffer, 0x00000000);
        bitmap_blit(buffer, &player, player_x, player_y);
        jcfb_refresh(buffer);
        usleep(1000);
    }

    bitmap_wipe(&player);
    bitmap_wipe(buffer);
    free(buffer);

    jcfb_stop();
    printf("Gentle quit\n");
    return 0;
}

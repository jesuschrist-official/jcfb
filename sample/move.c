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

    bitmap_t* screen = jcfb_get_bitmap();
    bitmap_clear(screen, 0x00000000);

    bitmap_t buffer;
    bitmap_init(&buffer, 640, 400);

    bitmap_t player;
    if (bitmap_load(&player, "data/player.bmp") != 0) {
        return 1;
    }
    int player_x = 640 / 2 - player.w / 2,
        player_y = 400 / 2 - player.h / 2;

    int exit = 0;
    while (!exit) {
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        if (is_key_pressed(KEYC_UP)) {
            player_y -= 16;
        }
        if (is_key_pressed(KEYC_DOWN)) {
            player_y += 16;
        }
        if (is_key_pressed(KEYC_LEFT)) {
            player_x -= 16;
        }
        if (is_key_pressed(KEYC_RIGHT)) {
            player_x += 16;
        }
        bitmap_clear(&buffer, 0x00000000);
        bitmap_masked_blit(&buffer, &player, player_x, player_y);
        bitmap_scaled_blit(screen, &buffer, 0, 0,
                           screen->w, screen->h);
        jcfb_refresh(screen);
        usleep(1000);
    }

    bitmap_wipe(&player);
    bitmap_wipe(&buffer);
    bitmap_wipe(screen);
    free(screen);

    jcfb_stop();
    printf("Gentle quit\n");
    return 0;
}

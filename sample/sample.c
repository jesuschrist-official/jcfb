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
    if (argc < 2) {
        fprintf(stderr, "usage: sample <image> \n");
        return 1;
    }

    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    bitmap_t image;
    if (bitmap_load(&image, argv[1]) != 0) {
        return 1;
    }

    bitmap_t player;
    if (bitmap_load(&player, "data/player.bmp") != 0) {
        return 1;
    }

    bitmap_t* buffer = jcfb_get_bitmap();
    bitmap_clear(buffer, 0x00000000);

    float ratio = 1.0;
    if (buffer->h < image.h || buffer->w < image.w) {
        float yratio = buffer->h / (float)image.h;
        float xratio = buffer->w / (float)image.w;
        if (image.w * yratio > buffer->w) {
            ratio = xratio;
        } else {
            ratio = yratio;
        }
    }
    int dw = image.w * ratio;
    int dh = image.h * ratio;
    int dx = (buffer->w - dw) / 2;
    int dy = (buffer->h - dh) / 2;
    bitmap_scaled_blit(buffer, &image, dx, dy, dw, dh);

    bitmap_masked_blit(buffer, &player, 600, 430);

    int exit = 0;
    while (!exit) {
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        jcfb_refresh(buffer);
        usleep(1000);
    }

    bitmap_wipe(&image);
    bitmap_wipe(&player);
    bitmap_wipe(buffer);
    free(buffer);

    jcfb_stop();
    printf("Gentle quit\n");
    return 0;
}

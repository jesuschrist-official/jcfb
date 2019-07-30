/*
 * Print an image on the screen.
 */
#include <stdio.h>
#include <unistd.h>
#include "jcfb/jcfb.h"


void draw(bitmap_t* buffer, bitmap_t* image, bool hflip) {
    bitmap_t tmp;
    bitmap_init(&tmp, image->w, image->h);
    if (hflip) {
        bitmap_blit_hflip(&tmp, image, 0, 0);
    } else {
        bitmap_blit(&tmp, image, 0, 0);
    }

    bitmap_clear(buffer, 0x00000000);

    float ratio = 1.0;
    if (buffer->h < image->h || buffer->w < image->w) {
        float yratio = buffer->h / (float)image->h;
        float xratio = buffer->w / (float)image->w;
        if (image->w * yratio > buffer->w) {
            ratio = xratio;
        } else {
            ratio = yratio;
        }
    }
    int dw = image->w * ratio;
    int dh = image->h * ratio;
    int dx = (buffer->w - dw) / 2;
    int dy = (buffer->h - dh) / 2;
    bitmap_scaled_blit(buffer, &tmp, dx, dy, dw, dh);
    bitmap_wipe(&tmp);
}


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

    bitmap_t buffer;
    jcfb_get_bitmap(&buffer);

    bool hflip = false;
    draw(&buffer, &image, hflip);

    int exit = 0;
    while (!exit) {
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        if (is_key_pressed(KEYC_SPACE)) {
            hflip = !hflip;
            draw(&buffer, &image, hflip);
        }
        jcfb_refresh(&buffer);
        usleep(1000);
    }

    bitmap_wipe(&image);
    bitmap_wipe(&buffer);

    jcfb_stop();
    return 0;
}

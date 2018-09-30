/*
 * Sample
 *
 * This source file shows how to use the ttf part of the library.
 */
#include <stdio.h>
#include <unistd.h>
#include "jcfb/jcfb.h"


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: ttf <font> \n");
        return 1;
    }

    ttf_font_t font;
    if (ttf_load(&font, argv[1]) < 0) {
        fprintf(stderr, "cannot load font file '%s'\n", argv[1]);
        return 1;
    }

    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    bitmap_t buffer;
    jcfb_get_bitmap(&buffer);
    bitmap_clear(&buffer, 0x00000000);

    ttf_render(&font,
               "Le vif zephir jubile sur les kumquats du clown "
               "gracieux",
               &buffer, 10, 24, 24,
               0x00cc5500);

    int base_line;
    float scale = stbtt_ScaleForPixelHeight(&font.font_info, 24);
    stbtt_GetFontVMetrics(&font.font_info, &base_line, NULL, NULL);
    base_line *= scale;
    draw_hline(&buffer, 0x00ff0000, 0, buffer.w - 1, 24);
    draw_hline(&buffer, 0x00ff0000, 0, buffer.w - 1, 24 + base_line);
    draw_hline(&buffer, 0x00ff0000, 0, buffer.w - 1, 48);

    int exit = 0;
    while (!exit) {
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        jcfb_refresh(&buffer);
        usleep(1000);
    }

    bitmap_wipe(&buffer);
    ttf_wipe(&font);

    jcfb_stop();
    return 0;
}

#include <stdio.h>

#include "jcfb/jcfb.h"


int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: convert <input> <output>\n");
        return 1;
    }

    if (jcfb_start() < 0) {
        fprintf(stderr, "cannot start JCFB\n");
        return 1;
    }

    bitmap_t image;
    if (bitmap_load(&image, argv[1]) < 0) {
        return 1;
    }
    if (bitmap_save(&image, argv[2]) < 0) {
        fprintf(stderr, "cannot save image as '%s'\n", argv[2]);
        return 1;
    }
    bitmap_wipe(&image);
    jcfb_stop();
    return 0;
}

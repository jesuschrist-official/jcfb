#include <stdio.h>
#include <sys/time.h>

#include "jcfb/bitmap.h"

#define NITERATIONS 100

int main(void) {
    bitmap_t bmp16, bmp24;

    bitmap_init_ex(&bmp16, PIXFMT_RGB16, 640, 480);
    bitmap_init_ex(&bmp24, PIXFMT_RGB24, 640, 480);

    struct timeval start, stop;

    gettimeofday(&start, NULL);
    for (volatile size_t i = 0; i < NITERATIONS; i++) {
        bitmap_blit(&bmp24, &bmp16, 0, 0);
    }
    gettimeofday(&stop, NULL);
    float elapsed = (stop.tv_sec + stop.tv_usec * 1E-6)
                  - (start.tv_sec + start.tv_usec * 1E-6);
    printf("Rate: %.2f blits/s\n", NITERATIONS * (1.0 / elapsed));

    bitmap_wipe(&bmp16);
    bitmap_wipe(&bmp24);

    return 0;
}

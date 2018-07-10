#include <stdio.h>
#include <sys/time.h>

#include "jcfb/pixel.h"

#define NITERATIONS 10000000

int main(void) {
    pixfmt_t rgb16 = pixfmt_get(PIXFMT_RGB16);
    pixfmt_t rgb24 = pixfmt_get(PIXFMT_RGB24);
    pixel_t p = 0x00f0f0f0;

    struct timeval start, stop;

    gettimeofday(&start, NULL);
    for (volatile size_t i = 0; i < NITERATIONS; i++) {
        pixel_conv(&rgb24, &rgb16, p);
    }
    gettimeofday(&stop, NULL);
    float elapsed = (stop.tv_sec + stop.tv_usec * 1E-6)
                  - (start.tv_sec + start.tv_usec * 1E-6);
    printf("Rate: %.2f pixels/s\n", NITERATIONS * (1.0 / elapsed));

    return 0;
}

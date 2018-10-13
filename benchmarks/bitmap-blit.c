#include <stdio.h>
#include <sys/time.h>


#include "jcfb/bitmap.h"


#define NITERATIONS 100
#define WIDTH   1024
#define HEIGHT  768


static float _slow_blit_bench() {
    bitmap_t bmp16, bmp24;

    bitmap_init_ex(&bmp16, PIXFMT_RGB16, 1920, 1080);
    bitmap_init_ex(&bmp24, PIXFMT_RGB24, 1920, 1080);

    struct timeval start, stop;

    gettimeofday(&start, NULL);
    for (volatile size_t i = 0; i < NITERATIONS; i++) {
        bitmap_blit(&bmp24, &bmp16, 0, 0);
    }
    gettimeofday(&stop, NULL);
    float elapsed = (stop.tv_sec + stop.tv_usec * 1E-6)
                  - (start.tv_sec + start.tv_usec * 1E-6);

    bitmap_wipe(&bmp16);
    bitmap_wipe(&bmp24);

    return NITERATIONS * (1.0f / elapsed);
}


static float _fast_blit_bench() {
    bitmap_t bmp_a, bmp_b;

    bitmap_init_ex(&bmp_a, PIXFMT_RGB24, 1920, 1080);
    bitmap_init_ex(&bmp_b, PIXFMT_RGB24, 1920, 1080);

    struct timeval start, stop;

    gettimeofday(&start, NULL);
    for (volatile size_t i = 0; i < NITERATIONS; i++) {
        bitmap_blit(&bmp_a, &bmp_b, 0, 0);
    }
    gettimeofday(&stop, NULL);
    float elapsed = (stop.tv_sec + stop.tv_usec * 1E-6)
                  - (start.tv_sec + start.tv_usec * 1E-6);

    bitmap_wipe(&bmp_a);
    bitmap_wipe(&bmp_b);

    return NITERATIONS * (1.0f / elapsed);
}


static float _scaled_blit_bench() {
    bitmap_t bmp_a, bmp_b;

    bitmap_init_ex(&bmp_a, PIXFMT_RGB24, 1024, 768);
    bitmap_init_ex(&bmp_b, PIXFMT_RGB24, 1920, 1080);

    struct timeval start, stop;

    gettimeofday(&start, NULL);
    for (volatile size_t i = 0; i < NITERATIONS; i++) {
        bitmap_scaled_blit(&bmp_a, &bmp_b, 0, 0, 1920, 1080);
    }
    gettimeofday(&stop, NULL);
    float elapsed = (stop.tv_sec + stop.tv_usec * 1E-6)
                  - (start.tv_sec + start.tv_usec * 1E-6);

    bitmap_wipe(&bmp_a);
    bitmap_wipe(&bmp_b);

    return NITERATIONS * (1.0f / elapsed);
}


static float _masked_blit_bench() {
    bitmap_t bmp_a, bmp_b;

    bitmap_init_ex(&bmp_a, PIXFMT_RGB24, 1024, 768);
    bitmap_init_ex(&bmp_b, PIXFMT_RGB24, 1920, 1080);

    struct timeval start, stop;

    gettimeofday(&start, NULL);
    for (volatile size_t i = 0; i < NITERATIONS; i++) {
        bitmap_masked_blit(&bmp_a, &bmp_b, 0, 0);
    }
    gettimeofday(&stop, NULL);
    float elapsed = (stop.tv_sec + stop.tv_usec * 1E-6)
                  - (start.tv_sec + start.tv_usec * 1E-6);

    bitmap_wipe(&bmp_a);
    bitmap_wipe(&bmp_b);

    return NITERATIONS * (1.0f / elapsed);
}


int main(void) {
    float slow_blit_rate = _slow_blit_bench();
    float fast_blit_rate = _fast_blit_bench();
    float scaled_blit_rate = _scaled_blit_bench();
    float masked_blit_rate = _masked_blit_bench();
    float ratio = fast_blit_rate / slow_blit_rate;

    printf("Slow blit rate:   %.2f blits/s\n"
           "Fast blit rate:   %.2f blits/s\n"
           "Scaled blit rate: %.2f blits/s\n"
           "Masked blit rate: %.2f blits/s\n"
           "Fast blit is %.2f times faster than slow blit\n",
           slow_blit_rate,
           fast_blit_rate,
           scaled_blit_rate,
           masked_blit_rate,
           ratio);

    return 0;
}

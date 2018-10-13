#include <stdio.h>
#include <sys/time.h>

#include "jcfb/jcfb.h"

#define NITERATIONS 1000

int main(void) {
    if (jcfb_start() < 0) {
        fprintf(stderr, "unable to start JCFB\n");
        return -1;
    }

    bitmap_t buffer;
    jcfb_get_bitmap(&buffer);
    bitmap_clear(&buffer, 0x005555EE);

    struct timeval start, stop;
    gettimeofday(&start, NULL);
    for (volatile size_t i = 0; i < NITERATIONS; i++) {
        jcfb_refresh(&buffer);
    }
    gettimeofday(&stop, NULL);

    bitmap_wipe(&buffer);
    jcfb_stop();

    float elapsed = (stop.tv_sec + stop.tv_usec * 1E-6)
                  - (start.tv_sec + start.tv_usec * 1E-6);
    printf("Rate: %.2f refresh/s\n",
           NITERATIONS * (1.0 / elapsed));

    return 0;
}

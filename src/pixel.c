#include <assert.h>

#include "jcfb/pixel.h"


static uint32_t _comp_conv(uint32_t off, uint32_t size, uint32_t comp) {
    assert(!(comp & 0xffffff00));
    if (8 > size) {
        comp >>= (8 - size);
    } else {
        comp <<= (size - 8);
    }
    return comp << off;
}


pixel_t pixel(const pixfmt_t* fmt, pixel_t rgba32) {
    return _comp_conv(fmt->offs[0], fmt->sizes[0], ((rgba32 >> 0 ) & 0xff))
         | _comp_conv(fmt->offs[1], fmt->sizes[1], ((rgba32 >> 8 ) & 0xff))
         | _comp_conv(fmt->offs[2], fmt->sizes[2], ((rgba32 >> 16) & 0xff))
         | _comp_conv(fmt->offs[3], fmt->sizes[3], ((rgba32 >> 24) & 0xff));
}

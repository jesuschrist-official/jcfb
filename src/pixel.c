#include <assert.h>
#include <string.h>

#include "jcfb/pixel.h"


static pixfmt_t _PIXFMTS[] = {
     [PIXFMT_FB] = {0},
     [PIXFMT_RGB16] = {
        .id = PIXFMT_RGB16,
        .bpp = 16,
        // XXX We chose red first, but maybe it's a wrong choice!
        //     Best would be to detect at bitmap loading if it is a LSB pixel
        //     format or not ?
        .offs = {11, 5, 0, 0},
        .sizes = {5, 6, 5, 0},
     },
     [PIXFMT_RGB24] = {
        .id = PIXFMT_RGB24,
        .bpp = 24,
        .offs = {16, 8, 0, 0},
        .sizes = {8, 8, 8, 0},
     },
     [PIXFMT_RGBA32] = {
        .id = PIXFMT_RGBA32,
        .bpp = 32,
        .offs = {24, 16, 8, 0},
        .sizes = {8, 8, 8, 8},
     },
     [PIXFMT_ARGB32] = {
        .id = PIXFMT_ARGB32,
        .bpp = 32,
        .offs = {16, 8, 0, 24},
        .sizes = {8, 8, 8, 8},
     },
};


pixfmt_t pixfmt_get(pixfmt_id_t which) {
    assert(which >= 0 && which < sizeof(_PIXFMTS) / sizeof(pixfmt_t));
    return _PIXFMTS[which];
}


void pixfmt_set_fb(const pixfmt_t* fmt) {
    assert(fmt->id == PIXFMT_FB);
    _PIXFMTS[PIXFMT_FB] = *fmt;
}


static uint32_t _comp_conv_32(uint32_t off, uint32_t size, uint32_t comp) {
    assert(!(comp & 0xffffff00));
    if (8 > size) {
        comp >>= (8 - size);
    } else {
        comp <<= (size - 8);
    }
    return comp << off;
}


pixel_t pixel(const pixfmt_t* fmt, pixel_t rgba32) {
    return _comp_conv_32(fmt->offs[0], fmt->sizes[0], ((rgba32 >> 0 ) & 0xff))
         | _comp_conv_32(fmt->offs[1], fmt->sizes[1], ((rgba32 >> 8 ) & 0xff))
         | _comp_conv_32(fmt->offs[2], fmt->sizes[2], ((rgba32 >> 16) & 0xff))
         | _comp_conv_32(fmt->offs[3], fmt->sizes[3], ((rgba32 >> 24) & 0xff));
}


static uint32_t _comp_conv(uint32_t in_off, uint32_t in_size,
                           uint32_t out_off, uint32_t out_size,
                           uint32_t in)
{
    uint32_t comp = (in >> in_off) & (~(UINT32_MAX << in_size));
    if (out_size > in_size) {
        comp <<= (out_size - in_size);
    } else {
        comp >>= (in_size - out_size);
    }
    return comp << out_off;
}


pixel_t pixel_conv(const pixfmt_t* in_fmt, const pixfmt_t* out_fmt, pixel_t p)
{
    if (in_fmt->id == out_fmt->id) {
        return p;
    }
#define CONV(_i) \
    _comp_conv(in_fmt->offs[_i], in_fmt->sizes[_i], \
               out_fmt->offs[_i], out_fmt->sizes[_i], p)
    return CONV(0) | CONV(1) | CONV(2) | CONV(3);
#undef CONV
}


#ifdef TEST

int main(void) {
    // TEST _comp_conv_32
    assert(_comp_conv_32(8, 5, 0xff) == 0x1f << 8); 
    assert(_comp_conv_32(8, 5, 0x00) == 0x00); 
    assert(_comp_conv_32(8, 16, 0xff) == 0xff00 << 8); 
    assert(_comp_conv_32(8, 4, 0xff) == 0x0f << 8); 

    // TEST _comp_conv
    // abcd are used for noise in following assertions
    assert(_comp_conv(8, 8, 8, 8, 0xabcdffab) == 0xff00);
    assert(_comp_conv(8, 8, 16, 4, 0xabcdffab) == 0xf0000);
    assert(_comp_conv(8, 4, 16, 8, 0xabcd0fab) == 0xf00000);

    pixfmt_t rgb16 = (pixfmt_t){
        .bpp = 16,
        .offs = {11, 5, 0, 0},
        .sizes = {5, 6, 5, 0}
    };
    pixfmt_t rgb24 = (pixfmt_t){
        .bpp = 24,
        .offs = {0, 8, 16, 0},
        .sizes = {8, 8, 8, 0}
    };

    pixel_t src, dst;

    // TEST pixel
    src = 0x04c08040;
    dst = pixel(&rgb16, src);
    // Red is rshifted by three (0x40 >> 3 == 0x08) and positioned at offset
    // 11. The maximal value of red is 2^6 - 1 == 0x1f.
    assert(((dst >> 11) & 0x1f) == 0x08);
    // Green is rshifted by 2 (0x80 >> 2 == 0x20) and positioned at offset
    // 5. The maximal value of green is 2^7 - 1 == 0x3f.
    assert(((dst >> 5) & 0x3f) == 0x20);
    // Blue is rshifted by 3 (0xc0 >> 3 == 0x18) and positioned at offset
    // 0. The maximal value of blue is 2^6 - 1 == 0x1f.
    assert(((dst >> 0) & 0x1f) == 0x18);
    // Pixels beyond 0xffff should be zero
    assert((dst & 0xffff0000) == 0);

    // TEST pixel_conv
    src = 0x04c08040;
    dst = pixel_conv(&rgb24, &rgb16, src);
    assert(((dst >> 11) & 0x1f) == 0x08);
    assert(((dst >> 5) & 0x3f) == 0x20);
    assert(((dst >> 0) & 0x1f) == 0x18);
    assert((dst & 0xffff0000) == 0);

    return 0;
}

#endif

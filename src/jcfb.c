#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "jcfb/jcfb.h"
#include "jcfb/pixel.h"


typedef struct fb {
    int fd;
    int w, h;
    pixfmt_t fmt;
    union {
        void* mem;
        uint8_t* mem8;
        uint16_t* mem16;
    };
} fb_t;


static fb_t _FB;


static size_t _FB_memsize() {
    return (_FB.fmt.bpp * _FB.w * _FB.h) / 8;
}


int jcfb_start() {
    _FB = (fb_t){
        .fd = -1,
        .mem = NULL,
    };

    _FB.fd = open("/dev/fb0", O_RDWR);
    if (_FB.fd < 0) {
        fprintf(stderr, "No framebuffer file descriptor");
        goto error;
    }

    struct fb_fix_screeninfo fix_si;
    struct fb_var_screeninfo var_si;
    if (ioctl(_FB.fd, FBIOGET_FSCREENINFO, &fix_si) < 0) {
        fprintf(stderr, "Unable to read framebuffer information");
        goto error;
    }
    if (ioctl(_FB.fd, FBIOGET_VSCREENINFO, &var_si) < 0) {
        fprintf(stderr, "Unable to read framebuffer information");
        goto error;
    }

    _FB.w = var_si.xres;
    _FB.h = var_si.yres;
    _FB.fmt = (pixfmt_t){
        .bpp = var_si.bits_per_pixel,
        .offs = {
            [COMP_RED] = var_si.red.offset,
            [COMP_GREEN] = var_si.green.offset,
            [COMP_BLUE] = var_si.blue.offset,
            [COMP_ALPHA] = var_si.transp.offset,
        },
        .sizes = {
            [COMP_RED] = var_si.red.length,
            [COMP_GREEN] = var_si.green.length,
            [COMP_BLUE] = var_si.blue.length,
            [COMP_ALPHA] = var_si.transp.length,
        }
    };

#ifdef DEBUG
    printf(
        "_FB = {\n"
        "   .w = %d,\n"
        "   .h = %d,\n"
        "   .fmt = {\n"
        "       .bpp = %zu,\n"
        "       .offs = {%u, %u, %u, %u},\n"
        "       .lengths = {%u, %u, %u, %u}\n"
        "   }\n"
        "}\n",
        _FB.w, _FB.h,
        _FB.fmt.bpp,
        _FB.fmt.offs[0], _FB.fmt.offs[1], _FB.fmt.offs[2], _FB.fmt.offs[3],
        _FB.fmt.sizes[0], _FB.fmt.sizes[1], _FB.fmt.sizes[2], _FB.fmt.sizes[3]
    );
#endif

    var_si.xoffset = 0;
    var_si.yoffset = 0;
    if (ioctl(_FB.fd, FBIOPAN_DISPLAY, &var_si) < 0) {
        fprintf(stderr, "Unable to modify the framebuffer offset");
        goto error;
    }

    _FB.mem = mmap(NULL, _FB_memsize(), PROT_READ | PROT_WRITE, MAP_SHARED,
                   _FB.fd, 0);
    if (_FB.mem == MAP_FAILED) {
        fprintf(stderr, "Unable to map framebuffer into program memory");
        goto error;
    }

    jcfb_clear();

    return 0;

  error:
    if (_FB.fd >= 0) close(_FB.fd);
    return -1;
}


void jcfb_stop() {
    if (_FB.mem) {
        munmap(_FB.mem, _FB_memsize());
        _FB.mem = NULL;
    }
    if (_FB.fd >= 0) {
        close(_FB.fd);
        _FB.fd = -1;
    }
}


void jcfb_clear() {
    pixel_t p = pixel(&_FB.fmt, 0x00000000);
    for (size_t i = 0; i < _FB.w * _FB.h; i++) {
        _FB.mem16[i] = p;
    }
}

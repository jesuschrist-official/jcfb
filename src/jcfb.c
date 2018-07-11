#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "jcfb/jcfb.h"
#include "jcfb/pixel.h"
#include "jcfb/bitmap.h"


typedef struct fb {
    int fd;
    void* mem;
    bitmap_t bmp;

    int page, page_max;
    struct fb_var_screeninfo var_si;
    struct fb_fix_screeninfo fix_si;
    bitmap_t backbuffer;    // Used only when max_page = 1
} fb_t;


static fb_t _FB;


static size_t _FB_memsize() {
    return (  _FB.var_si.xres_virtual
            * _FB.var_si.yres_virtual
            * _FB.var_si.bits_per_pixel) / 8;
}


static size_t _FB_pagesize() {
    return (_FB.var_si.xres * _FB.var_si.yres * _FB.var_si.bits_per_pixel) / 8;
}


static void _FB_set_page(int page) {
    assert(_FB.page_max > 1);
    _FB.page = page;
    _FB.var_si.xoffset = 0;
    _FB.var_si.yoffset = page * _FB.var_si.yres;
    // TODO error handling ?
    ioctl(_FB.fd, FBIOPAN_DISPLAY, &_FB.var_si);
    _FB.bmp.mem8 = _FB.mem + ((page + 1) % 2) * _FB_pagesize();
}


static bool _FB_has_double_buffering() {
    return _FB.page_max > 1;
}


int jcfb_start() {
    _FB = (fb_t){
        .fd = -1,
        .mem = NULL,
        .page = 0,
        .page_max = 0,
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

    int w = var_si.xres;
    int h = var_si.yres;
    pixfmt_t fmt = (pixfmt_t){
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
    assert((var_si.bits_per_pixel >= 8) && "BPP >= 8 is mandatory");

#ifdef DEBUG
    printf(
        "_FB.bmp = {\n"
        "   .w = %d,\n"
        "   .h = %d,\n"
        "   .fmt = {\n"
        "       .bpp = %zu,\n"
        "       .offs = {%u, %u, %u, %u},\n"
        "       .lengths = {%u, %u, %u, %u}\n"
        "   }\n"
        "}\n",
        w, h,
        fmt.bpp,
        fmt.offs[0], fmt.offs[1], fmt.offs[2], fmt.offs[3],
        fmt.sizes[0], fmt.sizes[1], fmt.sizes[2], fmt.sizes[3]
    );
#endif

    memcpy(&_FB.fix_si, &fix_si, sizeof(struct fb_fix_screeninfo));
    memcpy(&_FB.var_si, &var_si, sizeof(struct fb_var_screeninfo));

    _FB.mem = mmap(NULL, _FB_memsize(),
                   PROT_READ | PROT_WRITE, MAP_SHARED, _FB.fd, 0);
    if (_FB.mem == MAP_FAILED) {
        fprintf(stderr, "Unable to map framebuffer into program memory\n");
        goto error;
    }
    bitmap_init_from_memory(&_FB.bmp, &fmt, w, h, _FB.mem);

    memset(_FB.mem, 0, _FB_memsize());
    _FB.page_max = var_si.yres_virtual / var_si.yres;
    if (_FB_has_double_buffering()) {
        _FB_set_page(0);
    } else {
        bitmap_init(&_FB.backbuffer, &fmt, w, h);
    }

    return 0;

  error:
    if (_FB.fd >= 0) close(_FB.fd);
    return -1;
}


void jcfb_stop() {
    if (_FB.mem && _FB.mem != MAP_FAILED) {
        munmap(_FB.mem, _FB_memsize());
        _FB.mem = NULL;
    }
    if (_FB.fd >= 0) {
        close(_FB.fd);
        _FB.fd = -1;
    }
    if (!_FB_has_double_buffering()) {
        bitmap_wipe(&_FB.backbuffer);
    }
    bitmap_wipe(&_FB.bmp);
}


void jcfb_clear() {
    bitmap_clear(&_FB.bmp, 0x00000000);
}


bitmap_t* jcfb_get_bitmap() {
    if (_FB_has_double_buffering()) {
        return &_FB.bmp;
    } else {
        return &_FB.backbuffer;
    }
}


void jcfb_refresh() {
    if (_FB_has_double_buffering()) {
        _FB_set_page((_FB.page + 1) % 2);
    } else {
        memcpy(_FB.bmp.mem, _FB.backbuffer.mem, _FB_pagesize());
    }
}

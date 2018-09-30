#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/keyboard.h>


#include "jcfb/jcfb.h"
#include "jcfb/keyboard.h"
#include "jcfb/pixel.h"
#include "jcfb/bitmap.h"


#define MAX_KEY_QUEUE   128


typedef struct fb {
    int fd;
    void* mem;
    pixfmt_t fmt;

    int page, page_max;
    struct fb_var_screeninfo var_si;
    struct fb_fix_screeninfo fix_si;

    struct fb_var_screeninfo saved_var_si;
    struct fb_fix_screeninfo saved_fix_si;
} fb_t;


static fb_t _FB;


static void (*_sigsegv_handler)(int) = NULL;
static void (*_sigint_handler)(int) = NULL;
static void (*_sigabrt_handler)(int) = NULL;


static size_t _FB_memsize() {
    return (  _FB.var_si.xres_virtual
            * _FB.var_si.yres_virtual
            * _FB.var_si.bits_per_pixel) / 8;
}


static void _draw_frame(bitmap_t* bmp) {
    uint8_t* dest = _FB.mem;
    size_t bpp = _FB.var_si.bits_per_pixel / 8;
    pixel_t* src = bmp->mem;
    for (size_t i  = 0; i < bmp->w * bmp->h; i++) {
        pixel_t res = pixel_conv(PIXFMT_FB, bmp->fmt, *src);
        memcpy(dest, &res, bpp);
        dest += bpp;
        src++;
    }
}


static void _signal_handler(int signo) {
    jcfb_stop();
    if (signo == SIGSEGV) {
        signal(SIGSEGV, _sigsegv_handler);
        raise(SIGSEGV);
    } else
    if (signo == SIGINT) {
        signal(SIGINT, _sigint_handler);
        raise(SIGINT);
    } else
    if (signo == SIGABRT) {
        signal(SIGABRT, _sigabrt_handler);
        raise(SIGABRT);
    }
}


int jcfb_start() {
    _FB = (fb_t){
        .fd = -1,
        .mem = NULL,
        .page = 0,
        .page_max = 0,
    };

    // Retrieves framebuffer information
    struct fb_fix_screeninfo fix_si;
    struct fb_var_screeninfo var_si;
    _FB.fd = open("/dev/fb0", O_RDWR);
    if (_FB.fd < 0) {
        fprintf(stderr, "No framebuffer file descriptor\n");
        goto error;
    }
    if (ioctl(_FB.fd, FBIOGET_FSCREENINFO, &fix_si) < 0
    ||  ioctl(_FB.fd, FBIOGET_VSCREENINFO, &var_si) < 0)
    {
        fprintf(stderr, "Unable to read framebuffer information\n");
        goto error;
    }

    // Check for internal constraints
    if (var_si.bits_per_pixel < 8) {
        fprintf(stderr, "JCFB needs at least 8 bits of color depth\n");
        goto error;
    }

    // Framebuffer pixel format
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
    pixfmt_set_fb(&fmt);
    memcpy(&_FB.fmt, &fmt, sizeof(pixfmt_t));

    // Store framebuffer information in local memory
    memcpy(&_FB.fix_si, &fix_si, sizeof(struct fb_fix_screeninfo));
    memcpy(&_FB.var_si, &var_si, sizeof(struct fb_var_screeninfo));
    memcpy(&_FB.saved_fix_si, &fix_si,
           sizeof(struct fb_fix_screeninfo));
    memcpy(&_FB.saved_var_si, &var_si,
           sizeof(struct fb_var_screeninfo));

    // Map framebuffer memory
    _FB.mem = mmap(NULL, _FB_memsize(),
                   PROT_READ | PROT_WRITE, MAP_SHARED, _FB.fd, 0);
    if (_FB.mem == MAP_FAILED) {
        fprintf(stderr,
                "Unable to map framebuffer into program memory\n");
        goto error;
    }
    memset(_FB.mem, 0, _FB_memsize());

    // Scroll framebuffer to the start of its memory
    _FB.var_si.xoffset = 0;
    _FB.var_si.yoffset = 0;
    ioctl(_FB.fd, FBIOPUT_VSCREENINFO, &_FB.var_si);

    // Initialize the keyboard
    if (init_keyboard() < 0) {
        fprintf(stderr, "Unable to initialize the keyboard\n");
        goto error;
    }

    // Signals registration
    atexit(jcfb_stop);
    _sigsegv_handler = signal(SIGSEGV, _signal_handler);
    _sigint_handler = signal(SIGINT, _signal_handler);
    _sigabrt_handler = signal(SIGABRT, _signal_handler);

    return 0;

  error:
    jcfb_stop();
    return -1;
}


void jcfb_stop() {
    stop_keyboard();
    if (_FB.mem && _FB.mem != MAP_FAILED) {
        munmap(_FB.mem, _FB_memsize());
        _FB.mem = NULL;
    }
    if (_FB.fd >= 0) {
        ioctl(_FB.fd, FBIOPUT_VSCREENINFO, &_FB.saved_var_si);
        close(_FB.fd);
        _FB.fd = -1;
    }
}


void jcfb_clear() {
    memset(_FB.mem, 0, _FB_memsize());
}


int jcfb_get_bitmap(bitmap_t* bmp) {
    return bitmap_init(bmp, _FB.var_si.xres, _FB.var_si.yres);
}


void jcfb_refresh(bitmap_t* bmp) {
    if (bmp) {
        _draw_frame(bmp);
    }
    update_keyboard();
}


int jcfb_width() {
    return _FB.var_si.xres;
}


int jcfb_height() {
    return _FB.var_si.yres;
}

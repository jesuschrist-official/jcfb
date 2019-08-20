#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "jcfb/pixel.h"
#include "jcfb/bitmap-io.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"


// Loading ------------------------------------------------------------
int bitmap_load(bitmap_t* bmp, const char* path) {
    int w, h, n;
    pixel_t* data = (pixel_t*)stbi_load(path, &w, &h, &n, 4);
    if (!data) {
        fprintf(stderr, "Couldn't read image file '%s'\n", path);
        goto error;
    }
    if (bitmap_init(bmp, w, h) != 0) {
        goto error;
    }

    bool alpha = has_alpha(bmp->fmt);
    pixel_t* dst = bmp->mem;
    pixel_t* src = data;
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            // Inverse alpha-component (0 means full color, 1 no color)
            *src = (*src & 0x00ffffff) | ((0xff - (*src >> 24)) << 24);
            *dst = pixel_conv(PIXFMT_RGBA32, bmp->fmt, *src);
            if (!alpha && (*src & 0xff00ff) == 0xff00ff) {
                // XXX this can lead to bugs if RGB24 is not encoded
                //     as I think it should be :p
                *dst = 0xff000000;
            }
            dst++;
            src++;
        }
    }

    stbi_image_free(data);
    return 0;

  error:
    if (data) stbi_image_free(data);
    return 1;
}


// Saving -------------------------------------------------------------
enum imgfmt {
    IMGFMT_PNG,
    IMGFMT_BMP,
    IMGFMT_TGA,
    IMGFMT_JPG,
    IMGFMT_UNKNOWN,
};


const char _file_extensions[][16][16] = {
    [IMGFMT_PNG] = {"png", ""},
    [IMGFMT_BMP] = {"bmp", ""},
    [IMGFMT_TGA] = {"tga", ""},
    [IMGFMT_JPG] = {"jpg", "jpeg", ""},
};


static bool _has_ext(const char* path, const char* ext) {
    int path_len = strlen(path);
    int ext_len = strlen(ext);
    if (ext_len + 1 >= path_len) {
        return false;
    }
    if (path[path_len - 1 - ext_len] != '.') {
        return false;
    }
    // TODO handle case agnostic
    return !strcmp(path + path_len - ext_len, ext);
}


static enum imgfmt _find_fmt(const char* path) {
    if (_has_ext(path, "png")) {
        return IMGFMT_PNG;
    } else
    if (_has_ext(path, "bmp")) {
        return IMGFMT_BMP;
    } else
    if (_has_ext(path, "tga")) {
        return IMGFMT_TGA;
    }
    return IMGFMT_UNKNOWN;
}


static void* _prepare_data(const bitmap_t* bmp, pixfmt_id_t pixfmt) {
    size_t psize = pixfmt_get(pixfmt).bpp / 8;
    void* data = malloc(bmp->w * bmp->h * psize);
    for (int i = 0; i < bmp->w * bmp->h; i++) {
        pixel_t* pixel = (pixel_t*)(data + i * psize);
        *pixel = 0xff000000
               | pixel_conv(bmp->fmt, pixfmt, bmp->mem[i]);
        if ((bmp->mem[i] & 0xff000000) == 0xff000000) {
            *pixel = 0x00000000;
        }
    }
    return data;
}


int bitmap_save(const bitmap_t* bmp, const char* path) {
    void* data = NULL;
#define CHECK(_x) if (!_x) goto error;
    switch (_find_fmt(path)) {
      case IMGFMT_PNG:
        data = _prepare_data(bmp, PIXFMT_RGBA32);
        CHECK(stbi_write_png(path, bmp->w, bmp->h, 4, data, 0));
        break;

      case IMGFMT_BMP:
        data = _prepare_data(bmp, PIXFMT_RGB24);
        CHECK(stbi_write_bmp(path, bmp->w, bmp->h, 3, data));
        break;

      case IMGFMT_TGA:
        data = _prepare_data(bmp, PIXFMT_RGB24);
        CHECK(stbi_write_tga(path, bmp->w, bmp->h, 3, data));
        break;

      default:
        fprintf(stderr, "unsuported image file format '%s'\n'",
                        path);
        goto error;
    }
#undef CHECK

    if (data) free(data);
    return 0;

  error:
    if (data) free(data);
    return -1;
}


// --------------------------------------------------------------------

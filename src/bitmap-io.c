#include <stdint.h>
#include <stdio.h>

#include "jcfb/bitmap-io.h"


struct _header {
    uint8_t header_field[2];
    uint32_t size; 
    uint16_t reserved[2];
    uint32_t data_offset;
} __attribute__((packed));


// NOTE The DIB header "size" data is skipped for every following DIB header
//      data structure.
struct _core_header {
    uint16_t width;
    uint16_t height;
    uint16_t color_plane_count; /* Must be 1 */
    uint16_t bpp;
} __attribute__((packed));


struct _info_header {
    int32_t width;
    int32_t height;
    uint16_t color_plane_count; /* Must be 1 */
    uint16_t bpp;
    uint32_t compression_method;
    uint32_t image_size;
    int32_t horizontal_res; /* Pixels / meters */
    int32_t vertical_res;   /* Pixels / meters */
    uint32_t palette_size;
    uint32_t important_color_count;
} __attribute__((packed));


#define READ(_what) \
    if (fread(&_what, sizeof(_what), 1, f) != 1) { \
        fprintf(stderr, "Read error\n"); \
        goto error; \
    }


static int _bpp_to_pixfmt(uint16_t bpp, pixfmt_t* fmt) {
    switch (bpp) {
      case 16:
        *fmt = pixfmt_get(PIXFMT_RGB16);
        break;

      case 24:
        *fmt = pixfmt_get(PIXFMT_RGB24);
        break;

      case 32:
        *fmt = pixfmt_get(PIXFMT_RGBA32);
        break;

      default:
        fprintf(stderr, "Unsupported bits per pixel '%hu'\n", bpp);
        return -1;
    }
    return 0;
}


static int _load_core_header(FILE* f, bitmap_t* bmp) {
    struct _core_header header;
    pixfmt_t fmt;

    READ(header);
#ifdef DEBUG
    printf("CORE HEADER = {\n"
           "    .bpp = %hu\n"
           "    .w = %hu\n"
           "    .h = %hu\n"
           "}\n",
           header.bpp, header.width, header.height
    );
#endif
    if (_bpp_to_pixfmt(header.bpp, &fmt) < 0) {
        goto error;
    }
    if (bitmap_init(bmp, &fmt, header.width, header.height) < 0) {
        goto error;
    }

    return 0;

  error:
    return -1;
}


static int _load_info_header(FILE* f, bitmap_t* bmp) {
    struct _info_header header;
    pixfmt_t fmt;

    READ(header);
#ifdef DEBUG
    printf("INFO HEADER = {\n"
           "    .bpp = %hu\n"
           "    .w = %u\n"
           "    .h = %u\n"
           "    .compression_method = %u\n"
           "}\n",
           header.bpp, header.width, header.height, header.compression_method
    );
#endif
    if (header.compression_method != 0) {
        fprintf(stderr,
                "JCFB doesn't support bitmaps with compression method\n");
        goto error;
    }
    if (_bpp_to_pixfmt(header.bpp, &fmt) < 0) {
        goto error;
    }
    if (bitmap_init(bmp, &fmt, header.width, header.height) < 0) {
        goto error;
    }

    return 0;

  error:
    return -1;
}


static int _load_dib_header(FILE* f, uint32_t size, bitmap_t* bmp) {
    switch (size) {
      case 12:
        return _load_core_header(f, bmp);

      case 40:
        return _load_info_header(f, bmp);
      
      default:
        fprintf(stderr,
            "Unsupported bitmap type (supporting COREHEADER and "
            "INFOHEADER)\n"
        );
        return -1;
    }
}


static int _read_pixel_array(FILE* f, bitmap_t* bmp) {
    size_t bpp = bmp->fmt.bpp / 8;
    size_t padding = (bmp->w * bpp) % 4;   // XXX Only used when Height > 1 ?
    for (int y = bmp->h - 1; y >= 0; y--) {
        for (size_t x = 0; x < bmp->w; x++) {
            pixel_t p = 0;
            if (fread(&p, bpp, 1, f) != 1) {
                fprintf(stderr, "Cannot read pixel array\n");
                return -1;
            }
            bmp->mem[y * bmp->w + x] = p;
        }
        fseek(f, padding, SEEK_CUR);
    }
    return 0;
}


int bitmap_load(bitmap_t* bmp, const char* path) {
    struct _header header;
    FILE* f = NULL;

    f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Cannot load bitmap file '%s'\n", path);
        goto error;
    }

    READ(header);
    if (header.header_field[0] != 'B' || header.header_field[1] != 'M') {
        fprintf(stderr, "Unsupported bitmap type (%s must start with 'BM')\n",
                path);
        goto error;
    }
    uint32_t dib_header_size;
    READ(dib_header_size);
    if (_load_dib_header(f, dib_header_size, bmp) < 0) {
        fprintf(stderr, "Error reading DIB header of '%s'\n", path);
        goto error;
    }

    fseek(f, header.data_offset, SEEK_SET);
    if (_read_pixel_array(f, bmp) < 0) {
        fprintf(stderr, "Error reading pixel array of '%s'\n", path);
        goto error;
    }

    fclose(f);
    return 0;

  error:
    if (f) fclose(f);
    bitmap_wipe(bmp);
    return 1;
}

#if 0
int bitmap_save(const bitmap_t* bmp, const char* path) {
    FILE* f = fopen(path, "w+");
    if (!f) {
        fprintf(stderr, "Cannot open bitmap file '%s' for writing`\n",
                path);
        return 1;
    }

    size_t line_size = (bmp->w * bmp->bpp) / 8;
    line_size += (line_size % 4); // Padding
    size_t full_size = sizeof(struct _header) + sizeof(uint32_t)
                     + sizeof(struct _info_header)
                     + bmp->h * line_size;
    struct _header header = {
        .header_field = {'B', 'M'},
        .size = full_size,
        .reserved = {0},
        .data_offset = 0,
    };
    struct _info_header info_header = {
        .width = bmp->w,
        .height = bmp->h,
        .color_plane_count = 1,
        .bpp = bmp->fmt.bpp,
        .compression_method = 0,
        .image_size = 0,
        .horizontal_res = 96,
        .vertical_res = 96,
        .palette_size = 0,
        .important_color_count = 0,
    };

    fclose(f);
    return 0;

  error:
    fclose(f);
    return 1;
}
#endif


_Static_assert(
    sizeof(struct _header) == 14,
    "Your compiler doesn't support packing attribute"
);

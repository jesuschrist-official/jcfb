#include <stdio.h>
#include <string.h>
#include "jcfb/util.h"
#include "jcfb/ttf.h"


#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"


int ttf_load(ttf_font_t* ttf, const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "cannot open font file '%s'\n", path);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    ttf->buffer = malloc(size);
    if (!ttf->buffer) {
        goto error;
    }
    int res = fread(ttf->buffer, 1, size, f);
    if (res < 0) {
        goto error;
    }
    if (!stbtt_InitFont(&ttf->font_info, ttf->buffer, 0)) {
        goto error;
    }

    fclose(f);
    return 0;

  error:
    if (f) fclose(f);
    if (ttf->buffer) free(ttf->buffer);
    return -1;
}


void ttf_wipe(ttf_font_t* font) {
    free(font->buffer);
}


void ttf_render_cp(const ttf_font_t* font, int cp, bitmap_t* bmp,
                   int x, int y, int h, pixel_t color)
{
    float scale = stbtt_ScaleForPixelHeight(&font->font_info, h);
    int sw, sh, off_y;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        &font->font_info, 0, scale, cp, &sw, &sh, 0, &off_y
    );

    int base_line;
    stbtt_GetFontVMetrics(&font->font_info, &base_line, NULL, NULL);
    base_line *= scale;

    // TODO handle dy < 0 & sy < 0 too
    int diff = base_line + off_y;
    int dy = min(y + diff, bmp->h - 1);
    int sy = 0;
    for (; dy < bmp->h && sy < sh; dy++, sy++) {
        int dx = min(x, bmp->w - 1);
        int sx = 0;
        for (; dx < bmp->w && sx < sw; dx++, sx++) {
            if (!bitmap[sy * sw + sx]) {
                continue;
            }
            bitmap_put_pixel(bmp, dx, dy, color);
        }
    }

    free(bitmap);
}


void ttf_render(const ttf_font_t* font, const char* str, bitmap_t* bmp,
                int x, int y, int height, pixel_t color)
{
    float scale = stbtt_ScaleForPixelHeight(&font->font_info, height);
    for (int i = 0; i < strlen(str); i++) {
        int cp = str[i];
        ttf_render_cp(font, cp, bmp, x, y, height, color);

        int advance;
        stbtt_GetCodepointHMetrics(&font->font_info, cp, &advance,
                                   NULL);
        x += advance * scale * 1.2;
    }
}


int ttf_width_cp(const ttf_font_t* font, int cp, int height) {
    float scale = stbtt_ScaleForPixelHeight(&font->font_info, height);
    int advance;
    stbtt_GetCodepointHMetrics(&font->font_info, cp, &advance, NULL);
    return advance * scale * 1.2;
}


int ttf_width(const ttf_font_t* font, int height, const char* str,
              int size)
{
    if (size < 0) {
        size = strlen(str);
    }

    int width = 0;
    for (int i = 0; i < size - 1; i++) {
        width += ttf_width_cp(font, str[i], height);
    }
    if (size > 0) {
        width += ttf_width_cp(font, str[size - 1], height) * 1.3;
    }
    return width;
}

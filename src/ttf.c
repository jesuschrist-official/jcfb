#include <stdio.h>
#include <string.h>
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
    fread(ttf->buffer, 1, size, f);
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
                   int x, int y, int height, pixel_t color)
{
#if 0
STBTT_DEF void stbtt_GetFontVMetrics(const stbtt_fontinfo *info,
                                     int *ascent, int *descent,
                                     int *lineGap);
#endif
    float scale = stbtt_ScaleForPixelHeight(&font->font_info, height);
    int w, h, off_y;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        &font->font_info, 0, scale, cp, &w, &h, 0, &off_y
    );
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            if (!bitmap[dy * w + dx]) {
                continue;
            }
            bitmap_put_pixel(bmp, x + dx, y + dy + off_y, color);
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

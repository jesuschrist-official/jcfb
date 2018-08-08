/*
 * Wrapper around stb_truetype.h to handle TTF fonts.
 */
#ifndef _jcfb_ttf_h_
#define _jcfb_ttf_h_


#include "stb/stb_truetype.h"
#include "jcfb/bitmap.h"


typedef struct ttf_font {
    stbtt_fontinfo font_info;
    unsigned char* buffer;
} ttf_font_t;


/*
 * Load a truetype font.
 * Returns < 0 on error.
 */
int ttf_load(ttf_font_t* ttf, const char* path);


/*
 * Wipe a previously loaded font.
 */
void ttf_wipe(ttf_font_t* ttf);


/*
 * Render the given codepoint at the given position on `bmp` with a height
 * of `height` pixels using RGBA32 color `color`.
 */
void ttf_render_cp(const ttf_font_t* font, int cp, bitmap_t* bmp,
                   int x, int y, int height, pixel_t color);


/*
 * Same as above but on a string.
 * XXX Doesn't yet support UTF-8 encoded string.
 */
void ttf_render(const ttf_font_t* font, const char* str, bitmap_t* bmp,
                int x, int y, int height, pixel_t color);


#endif

#ifndef PRIMITIVE_PIXEL_FUNC
    #error "undefined PRIMITIVE_PIXEL_FUNC(dst, src) macro"
#endif

#ifndef PRIMITIVE_FUNC_SUFFIX
    #error "undefined PRIMITIVE_FUNC_SUFFIX"
#endif


#define __TCONCAT(x, y) x ## y
#define _TCONCAT(x, y) __TCONCAT(x, y)
#define FUNC(_name) _TCONCAT(_name, PRIMITIVE_FUNC_SUFFIX)


void FUNC(draw_hline)(bitmap_t* bmp, pixel_t color, int x1, int x2, int y) {
    if (y < 0 || y >= bmp->h || color == get_mask_color()) {
        return;
    }
    int x_min = max(min(x1, x2), 0);
    int x_max = min(max(x1, x2), bmp->w - 1);
    pixel_t* addr = bitmap_pixel_addr(bmp, x_min, y);
    for (int x = x_min; x <= x_max; x++) {
        PRIMITIVE_PIXEL_FUNC(*addr, color);
        addr++;
    }
}


void FUNC(draw_vline)(bitmap_t* bmp, pixel_t color, int x, int y1, int y2) {
    if (x < 0 || x >= bmp->w || color == get_mask_color()) {
        return;
    }
    int y_min = max(min(y1, y2), 0);
    int y_max = min(max(y1, y2), bmp->h - 1);
    pixel_t* addr = bitmap_pixel_addr(bmp, x, y_min);
    for (int y = y_min; y <= y_max; y++) {
        PRIMITIVE_PIXEL_FUNC(*addr, color);
        addr += bmp->w;
    }
}


void FUNC(draw_rect)(bitmap_t* bmp, pixel_t color, int x1, int y1,
                                                   int x2, int y2)
{
    int y_min = min(y1, y2);
    int y_max = max(y1, y2);
    int x_min = min(x1, x2);
    int x_max = max(x1, x2);
    FUNC(draw_hline)(bmp, color, x_min, x_max, y_min);
    FUNC(draw_hline)(bmp, color, x_min, x_max, y_max);
    FUNC(draw_vline)(bmp, color, x_min, y_min, y_max);
    FUNC(draw_vline)(bmp, color, x_max, y_min, y_max);
}


void FUNC(fill_rect)(bitmap_t* bmp, pixel_t color, int x1, int y1,
                                                   int x2, int y2)
{
    int y_min = min(y1, y2);
    int y_max = max(y1, y2);
    for (int y = y_min; y <= y_max; y++) {
        FUNC(draw_hline)(bmp, color, x1, x2, y);
    }
}


void FUNC(fill_circle)(bitmap_t* bmp, pixel_t color, int x, int y, int r) {
    int min_x = max(0, x - r);
    int max_x = min(x + r, bmp->w - 1);
    int min_y = max(0, y - r);
    int max_y = min(y + r, bmp->h - 1);
    for (int dy = min_y; dy <= max_y; dy++) {
        for (int dx = min_x; dx <= max_x; dx++) {
            if (_is_point_in_circle(x, y, r, dx, dy)) {
                pixel_t* addr = bitmap_pixel_addr(bmp, dx, dy);
                PRIMITIVE_PIXEL_FUNC(*addr, color);
            }
        }
    }
    FUNC(draw_circle)(bmp, color, x, y, r);
}


static void FUNC(_draw_circle)(bitmap_t* bmp, pixel_t color, int xc, int yc,
                               int x, int y)
{
    PRIMITIVE_PIXEL_FUNC(*bitmap_pixel_addr(bmp, xc + x, yc + y), color);
    PRIMITIVE_PIXEL_FUNC(*bitmap_pixel_addr(bmp, xc - x, yc - y), color);
    PRIMITIVE_PIXEL_FUNC(*bitmap_pixel_addr(bmp, xc + x, yc + y), color);
    PRIMITIVE_PIXEL_FUNC(*bitmap_pixel_addr(bmp, xc - x, yc - y), color);
    PRIMITIVE_PIXEL_FUNC(*bitmap_pixel_addr(bmp, xc + y, yc + x), color);
    PRIMITIVE_PIXEL_FUNC(*bitmap_pixel_addr(bmp, xc - y, yc - x), color);
    PRIMITIVE_PIXEL_FUNC(*bitmap_pixel_addr(bmp, xc + y, yc + x), color);
    PRIMITIVE_PIXEL_FUNC(*bitmap_pixel_addr(bmp, xc - y, yc - x), color);
}


void FUNC(draw_circle)(bitmap_t* bmp, pixel_t color, int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    FUNC(_draw_circle)(bmp, color, xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        FUNC(_draw_circle)(bmp, color, xc, yc, x, y);
    }
}


// Dashed functions ---------------------------------------------------
void FUNC(draw_dashed_hline)(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
                             int x1, int x2, int y,
                             int dash_start, int dash_length, int direction,
                             int stroke)
{
    if (y < 0 || y >= bmp->h) {
        return;
    }
    int x_min = max(min(x1, x2), 0);
    int x_max = min(max(x1, x2), bmp->w - 1);
    int length = (x_max - x_min) + dash_start;
    pixel_t* addr = bitmap_pixel_addr(bmp, x_min, y);
    for (int x = x_min; x <= x_max; x++) {
        pixel_t color = (length % (2 * dash_length) < dash_length)
                      ? color_a : color_b;
        FUNC(draw_vline)(bmp, color, x, y - stroke / 2, y + stroke / 2);
        addr++;
        length += direction;
    }
}


void FUNC(draw_dashed_vline)(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
                             int x, int y1, int y2,
                             int dash_start, int dash_length, int direction,
                             int stroke)
{
    int y_min = max(min(y1, y2), 0);
    int y_max = min(max(y1, y2), bmp->h - 1);
    int length = (y2 - y1) + dash_start;
    pixel_t* addr = bitmap_pixel_addr(bmp, x, y_min);
    for (int y = y_min; y <= y_max; y++) {
        pixel_t color = (length % (2 * dash_length) < dash_length)
                      ? color_a : color_b;
        FUNC(draw_hline)(bmp, color, x - stroke / 2, x + stroke / 2, y);
        addr += bmp->w;
        length += direction;
    }
}


void FUNC(draw_dashed_rect)(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
                            int x1, int y1, int x2, int y2,
                            int dash_start, int dash_length, int stroke)
{
    int y_min = min(y1, y2);
    int y_max = max(y1, y2);
    int x_min = min(x1, x2);
    int x_max = max(x1, x2);
    int w = x_max - x_min;
    int h = y_max - y_min;
    int length = dash_start;
    FUNC(draw_dashed_hline)(bmp, color_a, color_b, x_min, x_max, y_min,
                            length, dash_length, -1, stroke);
    length += w;
    FUNC(draw_dashed_vline)(bmp, color_a, color_b, x_max, y_min, y_max,
                            length, dash_length, -1, stroke);
    length += h;
    FUNC(draw_dashed_hline)(bmp, color_a, color_b, x_min, x_max, y_max,
                            length, dash_length, 1, stroke);
    length += w;
    FUNC(draw_dashed_vline)(bmp, color_a, color_b, x_min, y_min, y_max,
                            length, dash_length, 1, stroke);
    // Could not have a nice effect since bottom & left lines will
    // be drawn in a reverse direction than expected.
}


#undef PRIMITIVE_FUNC_SUFFIX
#undef PRIMITIVE_PIXEL_FUNC
#undef __TCONCAT
#undef _TCONCAT
#undef FUNC

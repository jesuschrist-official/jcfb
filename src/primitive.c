#include "jcfb/util.h"
#include "jcfb/primitive.h"


// Normal functions ---------------------------------------------------
void draw_hline(bitmap_t* bmp, pixel_t color, int x1, int x2, int y) {
    if (y < 0 || y >= bmp->h || color == get_mask_color()) {
        return;
    }
    int x_min = max(min(x1, x2), 0);
    int x_max = min(max(x1, x2), bmp->w - 1);
    pixel_t* addr = bitmap_pixel_addr(bmp, x_min, y);
    for (int x = x_min; x <= x_max; x++) {
        *addr = color;
        addr++;
    }
}


void draw_vline(bitmap_t* bmp, pixel_t color, int x, int y1, int y2) {
    if (x < 0 || x >= bmp->w || color == get_mask_color()) {
        return;
    }
    int y_min = max(min(y1, y2), 0);
    int y_max = min(max(y1, y2), bmp->h - 1);
    pixel_t* addr = bitmap_pixel_addr(bmp, x, y_min);
    for (int y = y_min; y <= y_max; y++) {
        *addr = color;
        addr += bmp->w;
    }
}


void draw_rect(bitmap_t* bmp, pixel_t color, int x1, int y1,
                                             int x2, int y2)
{
    int y_min = min(y1, y2);
    int y_max = max(y1, y2);
    int x_min = min(x1, x2);
    int x_max = max(x1, x2);
    draw_hline(bmp, color, x_min, x_max, y_min);
    draw_hline(bmp, color, x_min, x_max, y_max);
    draw_vline(bmp, color, x_min, y_min, y_max);
    draw_vline(bmp, color, x_max, y_min, y_max);
}


void fill_rect(bitmap_t* bmp, pixel_t color, int x1, int y1,
                                             int x2, int y2)
{
    int y_min = min(y1, y2);
    int y_max = max(y1, y2);
    for (int y = y_min; y <= y_max; y++) {
        draw_hline(bmp, color, x1, x2, y);
    }
}


static bool _is_point_in_circle(int cx, int cy, int r, int x, int y) {
    int dx = x - cx;
    int dy = y - cy;
    return dx * dx + dy * dy - r * r <= -1;
}


void fill_circle(bitmap_t* bmp, pixel_t color, int x, int y, int r) {
    int min_x = max(0, x - r);
    int max_x = min(x + r, bmp->w - 1);
    int min_y = max(0, y - r);
    int max_y = min(y + r, bmp->h - 1);
    for (int dy = min_y; dy <= max_y; dy++) {
        for (int dx = min_x; dx <= max_x; dx++) {
            if (_is_point_in_circle(x, y, r, dx, dy)) {
                pixel_t* addr = bitmap_pixel_addr(bmp, dx, dy);
                *addr = color;
            }
        }
    }
    draw_circle(bmp, color, x, y, r);
}


static void _draw_circle(bitmap_t* bmp, pixel_t color, int xc, int yc,
                         int x, int y)
{
    bitmap_put_pixel(bmp, xc + x, yc + y, color);
    bitmap_put_pixel(bmp, xc - x, yc + y, color);
    bitmap_put_pixel(bmp, xc + x, yc - y, color);
    bitmap_put_pixel(bmp, xc - x, yc - y, color);
    bitmap_put_pixel(bmp, xc + y, yc + x, color);
    bitmap_put_pixel(bmp, xc - y, yc + x, color);
    bitmap_put_pixel(bmp, xc + y, yc - x, color);
    bitmap_put_pixel(bmp, xc - y, yc - x, color);
}


void draw_circle(bitmap_t* bmp, pixel_t color, int xc, int yc, int r) {
        int x = 0, y = r;
        int d = 3 - 2 * r;
        _draw_circle(bmp, color, xc, yc, x, y);
        while (y >= x) {
            x++;
            if (d > 0) {
                y--;
                d = d + 4 * (x - y) + 10;
            } else {
                d = d + 4 * x + 6;
            }
            _draw_circle(bmp, color, xc, yc, x, y);
        }
}


// Dashed functions ---------------------------------------------------
void draw_dashed_hline(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
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
        draw_vline(bmp, color, x, y - stroke / 2, y + stroke / 2);
        addr++;
        length += direction;
    }
}


void draw_dashed_vline(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
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
        draw_hline(bmp, color, x - stroke / 2, x + stroke / 2, y);
        addr += bmp->w;
        length += direction;
    }
}


void draw_dashed_rect(bitmap_t* bmp, pixel_t color_a, pixel_t color_b,
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
    draw_dashed_hline(bmp, color_a, color_b, x_min, x_max, y_min,
                      length, dash_length, -1, stroke);
    length += w;
    draw_dashed_vline(bmp, color_a, color_b, x_max, y_min, y_max,
                      length, dash_length, -1, stroke);
    length += h;
    draw_dashed_hline(bmp, color_a, color_b, x_min, x_max, y_max,
                      length, dash_length, 1, stroke);
    length += w;
    draw_dashed_vline(bmp, color_a, color_b, x_min, y_min, y_max,
                      length, dash_length, 1, stroke);
    // Could not have a nice effect since bottom & left lines will
    // be drawn in a reverse direction than expected.
}
// --------------------------------------------------------------------

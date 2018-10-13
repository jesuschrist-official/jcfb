#include "jcfb/util.h"
#include "jcfb/primitive.h"


void draw_hline(bitmap_t* bmp, pixel_t color, int x1, int x2, int y) {
    int x_min = min(x1, x2);
    int x_max = max(x1, x2);
    pixel_t* addr = bitmap_pixel_addr(bmp, x_min, y);
    for (int x = x_min; x <= x_max; x++) {
        *addr = color;
        addr++;
    }
}


void draw_vline(bitmap_t* bmp, pixel_t color, int x, int y1, int y2) {
    int y_min = min(y1, y2);
    int y_max = max(y1, y2);
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
    for (int y = y_min; y <= y_max; y++) {
        draw_hline(bmp, color, x1, x2, y);
    }
}

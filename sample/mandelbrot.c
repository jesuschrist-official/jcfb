/*
 * Mandelbrot
 *
 * Display the mandelbrot fractal using the framebuffer.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "jcfb/jcfb.h"


// Mandelbrot -----------------------------------------------------------------
void rand_dimensions(float* xmin, float* xmax, float* ymin, float* ymax)
{
    float width = 3.5;
    float height = 2.0;

    do {
        *xmin = (-250 + rand() % 350) / 100.0;
        *xmax = (-250 + 100 + rand() % 250) / 100.0;
    } while (*xmax - *xmin == 0.0);

    if (*xmin > *xmax) {
        float tmp = *xmin;
        *xmin = *xmax;
        *xmax = tmp;
    }

    float xratio = (*xmax - *xmin) / width;
    float yheight = height * xratio;

    *ymin = (-100 + rand() % ((int) (yheight * 100) + 1)) / 100.0;
    *ymax = *ymin + yheight;
}


void mandelbrot(float cx, float cy, float sx, float sy,
                float* dx, float *dy)
{
    *dx = sx * sx - sy * sy + cx;
    *dy = 2.0 * sx * sy + cy;
}


int in_mandelbrot_set(float x, float y, float cx, float cy, float limit,
                      int nit_max)
{
    float nx, ny;

    int it = 0;

    // Ici, il y a surement moyen d'optimiser.
    // Il faut essayer de résoudre l'équation
    // mandelbrot(cx, cy, mandelbrot(..., mandelbrot(x, y))) >= limit^2
    while ((x * x) + (y * y) < limit * limit && it < nit_max) {
        mandelbrot(cx, cy, x, y, &nx, &ny);
        x = nx;
        y = ny;
        it++;
    }

    return it;
}


void generate(int width, int height, int* iterations, int it_max) {
    float xmin, xmax, ymin, ymax;
    float x0 = (rand() % 1000) * 0.001;
    float y0 = (rand() % 1000) * 0.001;
    float limit = 1.0 + (rand() % 100000) * 0.001;

    rand_dimensions(&xmin, &xmax, &ymin, &ymax);
    for (int yi = 0; yi < height; yi++) {
        for (int xi = 0; xi < width; xi++) {
            float cx = xmin + xi * ((xmax - xmin) / (float) width);
            float cy = ymin + yi * ((ymax - ymin) / (float) height);

            iterations[yi * width + xi] = in_mandelbrot_set(
                x0, y0, cx, cy, limit, it_max
            );
        }
    }
}


// Rendering ------------------------------------------------------------------
static int color_cos(int i, int size) {
    return (1 + cos((i / (float) size) * M_PI)) * 0.5 * 0xff;
}


static int color_sin(int i, int size) {
    return (1 + sin((i / (float) size) * M_PI)) * 0.5 * 0xff;
}


static int color_linear(int i, int size) {
    return i * (0xff / (float) size);
}


static int color_empty(int i, int size) {
    return 0;
}


static int color_full(int i, int size) {
    return 0xff;
}


static int (*color_funcs[5])(int, int) = {
    &color_cos,
    &color_sin,
    &color_linear,
    &color_empty,
    &color_full,
};


static void do_palette(pixel_t* colors, int size) {
    int (*rfunc)(int, int) = color_funcs[rand() % 5];
    int (*bfunc)(int, int) = color_funcs[rand() % 5];
    int (*gfunc)(int, int) = color_funcs[rand() % 5];

    for (int i = 0; i < size; i++) {
        colors[i] = rfunc(i, size)
                  | (gfunc(i, size) << 8)
                  | (bfunc(i, size) << 16);
    }
}


void draw(bitmap_t* bmp, const int* iterations, int it_max) {
    pixel_t palette[it_max];
    float k = it_max / log(it_max + 1);

    do_palette(palette, it_max);

    for (int yi = 0; yi < bmp->h; yi++) {
        for (int xi = 0; xi < bmp->w; xi++) {
            int it = iterations[yi * bmp->w + xi];
            bitmap_put_pixel(bmp, xi, yi, palette[(int)(k * log(it))]);
        }
    }
}


// ----------------------------------------------------------------------------
int main(int argc, char** argv) {
    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    srand(time(NULL));
    bitmap_t* buffer = jcfb_get_bitmap();

    int width = jcfb_width();
    int height = jcfb_height();
    int* iterations = malloc(width * height * sizeof(int));

    int nit = 1 + rand() % 100;
    generate(width, height, iterations, nit);
    draw(buffer, iterations, nit);

    int exit = 0;
    while (!exit) {
        if (jcfb_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        if (jcfb_key_pressed(KEYC_ENTER)) {
            nit = 1 + rand() % 100;
            generate(width, height, iterations, nit);
            draw(buffer, iterations, nit);
        }
        jcfb_refresh(buffer);
        usleep(1E6 / 30);
    }

    bitmap_wipe(buffer);
    free(buffer);
    jcfb_stop();
    return 0;
}

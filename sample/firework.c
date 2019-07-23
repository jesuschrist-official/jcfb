/*
 * Firework
 *
 * Firework for my daddy.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "jcfb/jcfb.h"


int rnd_int(int imin, int imax) {
    return imin + rand() % (imax - imin + 1);
}


float rnd_float(float fmin, float fmax) {
    return rnd_int(fmin * 10000, fmax * 10000) / 10000.0f;
}


typedef struct vec {
    float x, y;
} vec_t;


vec_t vec_add(vec_t u, vec_t v) {
    return (vec_t){
        .x = u.x + v.x,
        .y = u.y + v.y
    };
}


vec_t vec_sub(vec_t u, vec_t v) {
    return (vec_t){
        .x = u.x - v.x,
        .y = u.y - v.y
    };
}


vec_t vec_mul(vec_t u, float k) {
    return (vec_t){
        .x = u.x * k,
        .y = u.y * k
    };
}


vec_t vec_rotate(vec_t u, float a) {
    return (vec_t){
        .x = u.x * cos(a) - u.y * sin(a),
        .y = u.x * sin(a) + u.y * cos(a)
    };
}


typedef struct particle {
    int r, g, b;
    vec_t p; /* position */
    vec_t v; /* speed */
    int life, life_max;
} particle_t;


particle_t particle(vec_t p,
                    int r_min, int r_max,
                    int g_min, int g_max,
                    int b_min, int b_max,
                    float v_max,
                    float a_min, float a_max,
                    int life_min, int life_max)
{
    int life = rnd_int(life_min, life_max);
    vec_t v = (vec_t){rnd_float(0, v_max)};
    return (particle_t){
        .r = rnd_int(r_min, r_max),
        .g = rnd_int(g_min, g_max),
        .b = rnd_int(b_min, b_max),
        .p = p,
        .v = vec_rotate(v, rnd_float(a_min, a_max)),
        .life = life,
        .life_max = life
    };
}


void particle_update(particle_t* p) {
#define GRAVITY     0.03
    p->p = vec_add(p->p, p->v);
    p->v = vec_mul(p->v, 0.95);
    p->v.y += GRAVITY;
    p->life--;
}


void particle_render(bitmap_t* dst, const particle_t* p) {
    float k = p->life / (float)p->life_max;
    pixel_t color = rgb(p->r * k, p->g * k, p->b * k);
    bitmap_put_pixel_blend_add(dst, p->p.x, p->p.y, color);
}


#define MAX_PARTICLES 100000


typedef struct firework {
    size_t nparticles;
    particle_t particles[MAX_PARTICLES];
} firework_t;


void firework_gen_particle(firework_t* fw, size_t count,
                           vec_t p,
                           int r_min, int r_max,
                           int g_min, int g_max,
                           int b_min, int b_max,
                           float v_max,
                           float a_min, float a_max,
                           int life_min, int life_max)
{
    size_t limit = min(MAX_PARTICLES, fw->nparticles + count);
    for (size_t i = fw->nparticles; i < limit; i++) {
        fw->particles[i] = particle(
            p,
            r_min, r_max,
            g_min, g_max,
            b_min, b_max,
            v_max,
            a_min, a_max,
            life_min, life_max
        );
    }
    fw->nparticles = limit;
}


void firework_update(firework_t* fw) {
    for (size_t i = 0; i < fw->nparticles; i++) {
        particle_update(fw->particles + i);
        if (fw->particles[i].life <= 0) {
            memcpy(
                fw->particles + i, fw->particles + fw->nparticles - 1,
                sizeof(particle_t)
            );
            i--;
            fw->nparticles--;
        }
    } 
}


void firework_render(bitmap_t* dst, const firework_t* fw) {
    for (size_t i = 0; i < fw->nparticles; i++) {
        particle_render(dst, fw->particles + i);
    }
}


void feed(firework_t* fw, int w, int h) {
    vec_t p = (vec_t){
        rnd_float(w * 0.1, w * 0.9),
        rnd_float(h * 0.1, h * 0.9)
    };
    int r_min = rnd_int(32, 255);
    int r_max = rnd_int(r_min, 255);
    int g_min = rnd_int(32, 255);
    int g_max = rnd_int(g_min, 255);
    int b_min = rnd_int(32, 255);
    int b_max = rnd_int(b_min, 255);
    float v_max = rnd_float(1.0f, 20.0f);
    int life_min = rnd_int(10, 300);
    int life_max = rnd_int(life_min, 300);
    int count = rnd_int(1000, 10000);

    firework_gen_particle(
        fw, count,
        p,
        r_min, r_max,
        g_min, g_max,
        b_min, b_max,
        v_max,
        0, M_PI * 2,
        life_min, life_max
    );
}


int main(int argc, char** argv) {
    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    srand(time(NULL));
    bitmap_t buffer;
    jcfb_get_bitmap(&buffer);

    int width = jcfb_width();
    int height = jcfb_height();

    firework_t fw = {0};
    feed(&fw, width, height);

    int exit = 0;
    while (!exit) {
        if (rand() % 100 == 0) {
            feed(&fw, width, height);
        }

        bitmap_clear(&buffer, 0);
        firework_update(&fw);
        firework_render(&buffer, &fw);
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        if (is_key_pressed(KEYC_ENTER)) {
            feed(&fw, width, height);
        }
        jcfb_refresh(&buffer);
        usleep(1E6 / 60);
    }

    bitmap_wipe(&buffer);
    jcfb_stop();
    return 0;
}

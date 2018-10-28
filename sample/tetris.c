/*
 * Tetris game using JCFB for the display.
 */
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "jcfb/jcfb.h"

#define MAX(_x, _y) (((_x) > (_y)) ? (_x) : (_y))

// Tetrimino -----------------------------------------------------------
typedef enum {
    TETRIMINO_EMPTY = 0,
    TETRIMINO_I = 1,
    TETRIMINO_J,
    TETRIMINO_L,
    TETRIMINO_O,
    TETRIMINO_S,
    TETRIMINO_T,
    TETRIMINO_Z,
    TETRIMINO_MAX,
} tetrimino_id_t;


typedef struct tetrimino {
    tetrimino_id_t id;
    int w, h;
    char mask[6];   // Max size is 6 since max dimensions are (3, 2)
                    // for the L-shaped tetriminos
} tetrimino_t;


tetrimino_t tetriminoes[] = {
    [TETRIMINO_I] = {
        .id = TETRIMINO_I,
        .w = 1,
        .h = 4,
        .mask = {
            1,
            1,
            1,
            1,
            0, 0
        }
    },
    [TETRIMINO_J] = {
        .id = TETRIMINO_J,
        .w = 2,
        .h = 3,
        .mask = {
            0, 1,
            0, 1,
            1, 1,
        }
    },
    [TETRIMINO_L] = {
        .id = TETRIMINO_L,
        .w = 2,
        .h = 3,
        .mask = {
            1, 0,
            1, 0,
            1, 1,
        }
    },
    [TETRIMINO_O] = {
        .id = TETRIMINO_O,
        .w = 2,
        .h = 2,
        .mask = {
            1, 1,
            1, 1,
            0, 0,
        }
    },
    [TETRIMINO_S] = {
        .id = TETRIMINO_S,
        .w = 3,
        .h = 2,
        .mask = {
            0, 1, 1,
            1, 1, 0,
        }
    },
    [TETRIMINO_T] = {
        .id = TETRIMINO_T,
        .w = 3,
        .h = 2,
        .mask = {
            0, 1, 0,
            1, 1, 1,
        }
    },
    [TETRIMINO_Z] = {
        .id = TETRIMINO_Z,
        .w = 3,
        .h = 2,
        .mask = {
            1, 1, 0,
            0, 1, 1,
        }
    },
};


tetrimino_t get_tetrimino(tetrimino_id_t id) {
    return tetriminoes[id];
}


bool mask_at(const tetrimino_t* t, int x, int y) {
    return t->mask[y * t->w + x];
}


void set_mask(tetrimino_t* t, int x, int y, bool v) {
    t->mask[y * t->w + x] = v;
}


void clock_rotate_tetrimino(const tetrimino_t* t, tetrimino_t* r) {
    *r = (tetrimino_t){
        .id = t->id,
        .w = t->h,
        .h = t->w,
        .mask = {0}
    };

    for (int x = 0; x < t->w; x++) {
        for (int y = 0; y < t->h; y++) {
            set_mask(r, r->w - y - 1, x, mask_at(t, x, y));
        }
    }
}


// Game ---------------------------------------------------------------
/*
 * The board is 22x10 sized. First two lines are the top of the board
 * (where pieces spawn). They will be hidden when drawing.
 */
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 22

#define MAX_LEVELS 22


typedef struct game {
    tetrimino_t tetrimino;  // Currently played tetrimino
    int x, y;               // Current tetrimino position
    tetrimino_id_t board[BOARD_HEIGHT * BOARD_WIDTH];
    int lost;
    int loop_count;
    int loop_time;

    int level;
    int score;
    int lines_counter;
} game_t;


game_t _G;


/*
 * Return true if the piece at position (tx, ty) is at an invalid slot
 * on the board:
 * - the piece is out of the board ;
 * - the piece overlaps with a non-empty tile ;
 */
bool has_valid_position(int tx, int ty, const tetrimino_t* t) {
    // Check bounds
    int left = tx;
    int right = tx + t->w;
    int top = ty;
    int bottom = ty + t->h;
    if (left < 0 || right > BOARD_WIDTH || top < 0
    ||  bottom > BOARD_HEIGHT)
    {
        // Out of bounds
        return false;
    }

    // Check on board
    for (int y = 0; y < t->h; y++) {
        int dy = ty + y;
        for (int x = 0; x < t->w; x++) {
            if (!t->mask[y * t->w + x]) {
                // Empty mask, so no collision is possible
                continue;
            }
            int dx = tx + x;
            if (_G.board[dy * BOARD_WIDTH + dx] != TETRIMINO_EMPTY) {
                // There is overlap
                return false;
            }
        }
    }

    return true;
}


/*
 * Delete the given row
 */
void delete_row(int row) {
    memmove(_G.board + BOARD_WIDTH, _G.board,
            row * BOARD_WIDTH * sizeof(tetrimino_id_t));
}


/*
 * Lock the current tetrimino on the board.
 */
void lock_tetrimino() {
    for (int y = 0; y < _G.tetrimino.h; y++) {
        for (int x = 0; x < _G.tetrimino.w; x++) {
            if (!_G.tetrimino.mask[y * _G.tetrimino.w + x]) {
                continue;
            }
            tetrimino_id_t id = _G.tetrimino.id;
            _G.board[(_G.y + y) * BOARD_WIDTH + (_G.x + x)] = id;
        }
    }

    // Check if we need to delete some rows
    int deleted = 0;
    for (int y = _G.y; y < _G.y + _G.tetrimino.h; y++) {
        int count = 0;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (_G.board[y * BOARD_WIDTH + x] != TETRIMINO_EMPTY) {
                count++;
            }
        }
        if (count == BOARD_WIDTH) {
            delete_row(y);
            deleted++;
        }
    }

    // Increasing score
    static int lines_to_score[] = {0, 40, 100, 300, 1200};
    _G.score += lines_to_score[deleted] * (_G.level + 1);

    // Increasing level
    if (_G.lines_counter + deleted >= 10) {
        _G.level++;
    }
    _G.lines_counter = (_G.lines_counter + deleted) % 10;
}


/*
 * Apply gravity. Returns false if the current tetrimino become locked.
 */
bool apply_gravity() {
    if (!has_valid_position(_G.x, _G.y + 1, &_G.tetrimino)) {
        return false;
    } else {
        _G.y++;
        return true;
    }
}


/*
 * Spawn a new tetrimino
 */
void spawn_tetrimino() {
    tetrimino_id_t id = 1 + (rand() % (TETRIMINO_MAX - 1));
    _G.tetrimino = tetriminoes[id];
    _G.x = (BOARD_WIDTH / 2) - (_G.tetrimino.w / 2);
    _G.y = 0;
}


/*
 * Start the game
 */
void start_game() {
    memset(&_G.tetrimino, 0, sizeof(tetrimino_t));
    memset(_G.board, 0,
           BOARD_WIDTH * BOARD_HEIGHT * sizeof(tetrimino_id_t));
    spawn_tetrimino();
    _G.lost = false;
    _G.loop_count = 0;
    _G.level = 0;
    _G.score = 0;
    _G.lines_counter = 0;
}


/*
 * Loop the game
 */
void loop_game() {
    int mod_op = MAX(1, MAX_LEVELS - _G.level);
    _G.loop_count = (_G.loop_count + 1) % mod_op;
    if (_G.loop_count) {
        return;
    }
    if (!apply_gravity()) {
        lock_tetrimino();
        spawn_tetrimino();
        if (!has_valid_position(_G.x, _G.y, &_G.tetrimino)) {
            _G.lost = true;
            return;
        }
    }
}


/*
 * Move the tetrimino left
 */
void move_left() {
    if (has_valid_position(_G.x - 1, _G.y, &_G.tetrimino)) {
        _G.x--;
    }
}


/*
 * Move the tetrimino right
 */
void move_right() {
    if (has_valid_position(_G.x + 1, _G.y, &_G.tetrimino)) {
        _G.x++;
    }
}


/*
 * Rotate the tetrimino clockwise
 */
void rotate_clock() {
    tetrimino_t r;
    clock_rotate_tetrimino(&_G.tetrimino, &r);
    if (has_valid_position(_G.x, _G.y, &r)) {
        _G.tetrimino = r;
    }
}


/*
 * Hard drop the current tetrimino
 */
void hard_drop() {
    while (apply_gravity());
    _G.loop_count = 1;
}


// Display ------------------------------------------------------------
enum colors {
    CYAN = 0x0077ff00,
    YELLOW = 0x0000ffff,
    PURPLE = 0x00ff00ff,
    GREEN = 0x0000ff00,
    RED = 0x000000ff,
    BLUE = 0x00ff0000,
    ORANGE = 0x000088ff,
};


pixel_t tetrimino_colors[] = {
    [TETRIMINO_I] = CYAN,
    [TETRIMINO_O] = YELLOW,
    [TETRIMINO_T] = PURPLE,
    [TETRIMINO_S] = GREEN,
    [TETRIMINO_Z] = RED,
    [TETRIMINO_J] = BLUE,
    [TETRIMINO_L] = ORANGE,
};


void draw_tile(bitmap_t* bmp, int x, int y, tetrimino_id_t id) {
    if (y < 2) {
        // Don't draw tiles on first two lines.
        return;
    }
    float res_w = bmp->w / (float)BOARD_WIDTH;
    float res_h = bmp->h / (float)(BOARD_HEIGHT - 2);

    int x1 = x * res_w + 1;
    int x2 = (x + 1) * res_w - 1;
    int y1 = (y - 2) * res_h + 1;   // -2 is to adapt coordinates
    int y2 = (y - 1) * res_h - 1;

    fill_rect(bmp, pixel(tetrimino_colors[id]), x1, y1, x2, y2);
}


void draw_game(bitmap_t* bmp) {
    bitmap_clear(bmp, 0x00000000); 
    for (int y = 2; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            tetrimino_id_t id = _G.board[y * BOARD_WIDTH + x];
            if (id != TETRIMINO_EMPTY) {
                draw_tile(bmp, x, y, id);
            }
        }
    }

    for (int y = 0; y < _G.tetrimino.h; y++) {
        for (int x = 0; x < _G.tetrimino.w; x++) {
            if (!_G.tetrimino.mask[y * _G.tetrimino.w + x]) {
                continue;
            }
            draw_tile(bmp, x + _G.x, y + _G.y, _G.tetrimino.id);
        }
    }
}


// --------------------------------------------------------------------
int main(int argc, char** argv) {
    srand(time(NULL));

    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    bitmap_t buffer;
    jcfb_get_bitmap(&buffer);
    bitmap_t board;
    bitmap_init(&board, buffer.h / 2, buffer.h - 1);

    ttf_font_t font;
    if (ttf_load(&font, "data/Hermit-light.otf") < 0) {
        goto error;
    }

    start_game();
    int exit = 0;
    while (!exit) {
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        if (is_key_pressed(KEYC_LEFT)) {
            move_left();
        } else
        if (is_key_pressed(KEYC_RIGHT)) {
            move_right();
        } else
        if (is_key_pressed(KEYC_R) || is_key_pressed(KEYC_UP)) {
            rotate_clock();
        } else
        if (is_key_pressed(KEYC_DOWN)) {
            hard_drop();
        }
        loop_game();

        bitmap_clear(&buffer, 0x00000000);
        draw_vline(&buffer, pixel(0x00ffffff),
                   buffer.w / 2 - board.w / 2 - 1,
                   0, buffer.h);
        draw_vline(&buffer, pixel(0x00ffffff),
                   buffer.w / 2 + board.w / 2 + 1,
                   0, buffer.h);
        draw_hline(&buffer, pixel(0x00ffffff),
                   buffer.w / 2 - board.w / 2,
                   buffer.w / 2 + board.w / 2 + 1,
                   buffer.h - 1);

        draw_game(&board);
        bitmap_blit(&buffer, &board, buffer.w / 2 - board.w / 2, 0);

        char str_buffer[256];
        sprintf(str_buffer, "Level %d", _G.level);
        ttf_render(&font, str_buffer, &buffer,
                   buffer.w / 2 + board.w / 2 + 10,
                   24, 24, 0x0055cc22);
        sprintf(str_buffer, "Score %d", _G.score);
        ttf_render(&font, str_buffer, &buffer,
                   buffer.w / 2 + board.w / 2 + 10,
                   48, 24, 0x00cc5500);

        jcfb_refresh(&buffer);
        usleep(1E6 / 30);
    }

    bitmap_wipe(&board);
    bitmap_wipe(&buffer);
    ttf_wipe(&font);
    jcfb_stop();

    return 0;

  error:
    jcfb_stop();
    return 1;
}

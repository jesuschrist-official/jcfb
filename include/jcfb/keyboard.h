/*
 * JCFB keyboard handling.
 */
#ifndef _jcfb_keyboard_h_
#define _jcfb_keyboard_h_


#include <stdbool.h>


/*
 * Key code type (see below).
 */
typedef enum keyc keyc_t;


/*
 * Set terminal mode & configuration.
 * Returns negative value on failure.
 */
int init_keyboard();


/*
 * Restore previous terminal state.
 */
void stop_keyboard();


/*
 * Update keyboard's keys state.
 */
void update_keyboard();


/*
 * Check if the given key is pressed.
 */
bool is_key_pressed(keyc_t key);


/*
 * Check if shift is pressed.
 */
bool is_shift_pressed(keyc_t key);


// JCFB keycodes ------------------------------------------------------
enum keyc {
    KEYC_UNKNOWN = -1,

    KEYC_TAB = 9,
    KEYC_ENTER = 10,
    KEYC_ESC = 27,

    /* Printable characters, mapped on the ASCII table */
    KEYC_SPACE = 32,
    KEYC_BANG,
    KEYC_QUOTE,
    KEYC_SHARP,
    KEYC_DOLLAR,
    KEYC_PERCENT,
    KEYC_AND,
    KEYC_SINGLE_QUOTE,
    KEYC_LPARENTHESIS,
    KEYC_RPARENTHESIS,
    KEYC_STAR,
    KEYC_PLUS,
    KEYC_COMMA,
    KEYC_DASH,
    KEYC_DOT,
    KEYC_SLASH,
    KEYC_0,
    KEYC_1,
    KEYC_2,
    KEYC_3,
    KEYC_4,
    KEYC_5,
    KEYC_6,
    KEYC_7,
    KEYC_8,
    KEYC_9,
    KEYC_DOUBLE_DOT,
    KEYC_SEMICOLON,
    KEYC_LT,
    KEYC_EQUAL,
    KEYC_GT,
    KEYC_QUESTION_MARK,
    KEYC_AT,
    KEYC_A,
    KEYC_B,
    KEYC_C,
    KEYC_D,
    KEYC_E,
    KEYC_F,
    KEYC_G,
    KEYC_H,
    KEYC_I,
    KEYC_J,
    KEYC_K,
    KEYC_L,
    KEYC_M,
    KEYC_N,
    KEYC_O,
    KEYC_P,
    KEYC_Q,
    KEYC_R,
    KEYC_S,
    KEYC_T,
    KEYC_U,
    KEYC_V,
    KEYC_W,
    KEYC_X,
    KEYC_Y,
    KEYC_Z,
    KEYC_LBRACKET,
    KEYC_BACKSLASH,
    KEYC_RBRACKET,
    KEYC_CIRCUMFLEX,
    KEYC_UNDERSCORE,
    KEYC_GRAVE_ACCENT,
    KEYC_a,
    KEYC_b,
    KEYC_c,
    KEYC_d,
    KEYC_e,
    KEYC_f,
    KEYC_g,
    KEYC_h,
    KEYC_i,
    KEYC_j,
    KEYC_k,
    KEYC_l,
    KEYC_m,
    KEYC_n,
    KEYC_o,
    KEYC_p,
    KEYC_q,
    KEYC_r,
    KEYC_s,
    KEYC_t,
    KEYC_u,
    KEYC_v,
    KEYC_w,
    KEYC_x,
    KEYC_y,
    KEYC_z,
    KEYC_LBRACE,
    KEYC_PIPE,
    KEYC_RBRACE,
    KEYC_TILDE,
    KEYC_DELETE,

    KEYC_CTRL,
    KEYC_SHIFT,
    KEYC_ALT,
    KEYC_ALTGR,
    KEYC_BACKSPACE,
    KEYC_PRINT,
    KEYC_F1,
    KEYC_F2,
    KEYC_F3,
    KEYC_F4,
    KEYC_F5,
    KEYC_F6,
    KEYC_F7,
    KEYC_F8,
    KEYC_F9,
    KEYC_F10,
    KEYC_F11,
    KEYC_F12,
    KEYC_LEFT,
    KEYC_RIGHT,
    KEYC_UP,
    KEYC_DOWN,
    KEYC_SUPPR,
    KEYC_INSER,
    KEYC_BEGIN,
    KEYC_PAGE_UP,
    KEYC_PAGE_DOWN,
    KEYC_END,
    KEYC_PAUSE,
    KEYC_LWINDOW,
    KEYC_RWINDOW,
    KEYC_CAPSLOCK,
    KEYC_NUMLOCK,
    KEYC_HOLD,

    KEYC_ACUTE,
    KEYC_DIERE,
    KEYC_CEDIL,

    KEYC_MAX = 512
};


// Keyboard user interface --------------------------------------------
typedef enum {
    KEYBEVT_PRESSED,
    KEYBEVT_HELD,
    KEYBEVT_RELEASED,
} keybevt_type_t;


typedef struct keybevt {
    keybevt_type_t type;
    keyc_t keyc;
} keybevt_t;


/**
 * Poll next keyboard event.
 * Returns 0 if there is no more event to poll.
 */
int poll_keyboard(keybevt_t* evt);


// --------------------------------------------------------------------


#endif

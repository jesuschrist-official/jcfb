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


// JCFB keycodes ------------------------------------------------------
enum keyc {
    KEYC_UNKNOWN = -1,

    KEYC_TAB = 9,
    KEYC_ENTER = 10,

    KEYC_ESC = 27,

    KEYC_SPACE = 32,
    KEYC_BANG = 33,
    KEYC_QUOTE = 34,
    KEYC_SHARP = 35,
    KEYC_DOLLAR = 36,
    KEYC_PERCENT = 37,
    KEYC_AND = 38,
    KEYC_SINGLE_QUOTE = 39,
    KEYC_LPARENTHESIS = 40,
    KEYC_RPARENTHESIS = 41,
    KEYC_STAR = 42,
    KEYC_PLUS = 43,
    KEYC_COMMA = 44,
    KEYC_DASH = 45,
    KEYC_DOT = 46,
    KEYC_SLASH = 47,

    KEYC_0 = 48,
    KEYC_1,
    KEYC_2,
    KEYC_3,
    KEYC_4,
    KEYC_5,
    KEYC_6,
    KEYC_7,
    KEYC_8,
    KEYC_9,

    KEYC_DOUBLE_DOT = 58,
    KEYC_SEMICOLON = 59,
    KEYC_LT = 60,
    KEYC_EQUAL = 61,
    KEYC_GT = 62,
    KEYC_QUESTION_MARK = 63,
    KEYC_AT = 64,

    KEYC_LBRACKET = 91,
    KEYC_BACKSLASH = 92,
    KEYC_RBRACKET = 93,
    KEYC_CIRCUMFLEX = 94,
    KEYC_UNDERSCORE = 95,
    KEYC_GRAVE_ACCENT = 96,

    KEYC_A = 97,
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

    KEYC_ACUTE,
    KEYC_DIERE,
    KEYC_CEDIL,

    KEYC_HOLD,

    KEYC_MAX = 512
};
// --------------------------------------------------------------------


#endif

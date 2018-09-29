#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>


#include "jcfb/keyboard.h"


#define MAXEVTS 1024


// Key names (debug) --------------------------------------------------
#ifdef KEYB_DEBUG
static const char* key_names[KEYC_MAX] = {
    [KEYC_TAB] = "KEYC_TAB",
    [KEYC_ENTER] = "KEYC_ENTER",
    [KEYC_ESC] = "KEYC_ESC",

    [KEYC_SPACE] = "KEYC_SPACE",
    [KEYC_BANG] = "KEYC_BANG",
    [KEYC_QUOTE] = "KEYC_QUOTE",
    [KEYC_SHARP] = "KEYC_SHARP",
    [KEYC_DOLLAR] = "KEYC_DOLLAR",
    [KEYC_PERCENT] = "KEYC_PERCENT",
    [KEYC_AND] = "KEYC_AND",
    [KEYC_SINGLE_QUOTE] = "KEYC_SINGLE_QUOTE",
    [KEYC_LPARENTHESIS] = "KEYC_LPARENTHESIS",
    [KEYC_RPARENTHESIS] = "KEYC_RPARENTHESIS",
    [KEYC_STAR] = "KEYC_STAR",
    [KEYC_PLUS] = "KEYC_PLUS",
    [KEYC_COMMA] = "KEYC_COMMA",
    [KEYC_DASH] = "KEYC_DASH",
    [KEYC_DOT] = "KEYC_DOT",
    [KEYC_SLASH] = "KEYC_SLASH",
    [KEYC_0] = "KEYC_0",
    [KEYC_1] = "KEYC_1",
    [KEYC_2] = "KEYC_2",
    [KEYC_3] = "KEYC_3",
    [KEYC_4] = "KEYC_4",
    [KEYC_5] = "KEYC_5",
    [KEYC_6] = "KEYC_6",
    [KEYC_7] = "KEYC_7",
    [KEYC_8] = "KEYC_8",
    [KEYC_9] = "KEYC_9",
    [KEYC_DOUBLE_DOT] = "KEYC_DOUBLE_DOT",
    [KEYC_SEMICOLON] = "KEYC_SEMICOLON",
    [KEYC_LT] = "KEYC_LT",
    [KEYC_EQUAL] = "KEYC_EQUAL",
    [KEYC_GT] = "KEYC_GT",
    [KEYC_QUESTION_MARK] = "KEYC_QUESTION_MARK",
    [KEYC_AT] = "KEYC_AT",
    [KEYC_A] = "KEYC_A",
    [KEYC_B] = "KEYC_B",
    [KEYC_C] = "KEYC_C",
    [KEYC_D] = "KEYC_D",
    [KEYC_E] = "KEYC_E",
    [KEYC_F] = "KEYC_F",
    [KEYC_G] = "KEYC_G",
    [KEYC_H] = "KEYC_H",
    [KEYC_I] = "KEYC_I",
    [KEYC_J] = "KEYC_J",
    [KEYC_K] = "KEYC_K",
    [KEYC_L] = "KEYC_L",
    [KEYC_M] = "KEYC_M",
    [KEYC_N] = "KEYC_N",
    [KEYC_O] = "KEYC_O",
    [KEYC_P] = "KEYC_P",
    [KEYC_Q] = "KEYC_Q",
    [KEYC_R] = "KEYC_R",
    [KEYC_S] = "KEYC_S",
    [KEYC_T] = "KEYC_T",
    [KEYC_U] = "KEYC_U",
    [KEYC_V] = "KEYC_V",
    [KEYC_W] = "KEYC_W",
    [KEYC_X] = "KEYC_X",
    [KEYC_Y] = "KEYC_Y",
    [KEYC_Z] = "KEYC_Z",
    [KEYC_LBRACKET] = "KEYC_LBRACKET",
    [KEYC_BACKSLASH] = "KEYC_BACKSLASH",
    [KEYC_RBRACKET] = "KEYC_RBRACKET",
    [KEYC_CIRCUMFLEX] = "KEYC_CIRCUMFLEX",
    [KEYC_UNDERSCORE] = "KEYC_UNDERSCORE",
    [KEYC_GRAVE_ACCENT] = "KEYC_GRAVE_ACCENT",
    [KEYC_a] = "KEYC_a",
    [KEYC_b] = "KEYC_b",
    [KEYC_c] = "KEYC_c",
    [KEYC_d] = "KEYC_d",
    [KEYC_e] = "KEYC_e",
    [KEYC_f] = "KEYC_f",
    [KEYC_g] = "KEYC_g",
    [KEYC_h] = "KEYC_h",
    [KEYC_i] = "KEYC_i",
    [KEYC_j] = "KEYC_j",
    [KEYC_k] = "KEYC_k",
    [KEYC_l] = "KEYC_l",
    [KEYC_m] = "KEYC_m",
    [KEYC_n] = "KEYC_n",
    [KEYC_o] = "KEYC_o",
    [KEYC_p] = "KEYC_p",
    [KEYC_q] = "KEYC_q",
    [KEYC_r] = "KEYC_r",
    [KEYC_s] = "KEYC_s",
    [KEYC_t] = "KEYC_t",
    [KEYC_u] = "KEYC_u",
    [KEYC_v] = "KEYC_v",
    [KEYC_w] = "KEYC_w",
    [KEYC_x] = "KEYC_x",
    [KEYC_y] = "KEYC_y",
    [KEYC_z] = "KEYC_z",
    [KEYC_LBRACE] = "KEYC_LBRACE",
    [KEYC_PIPE] = "KEYC_PIPE",
    [KEYC_RBRACE] = "KEYC_RBRACE",
    [KEYC_TILDE] = "KEYC_TILDE",
    [KEYC_DELETE] = "KEYC_DELETE",

    [KEYC_CTRL] = "KEYC_CTRL",
    [KEYC_SHIFT] = "KEYC_SHIFT",
    [KEYC_ALT] = "KEYC_ALT",
    [KEYC_ALTGR] = "KEYC_ALTGR",
    [KEYC_BACKSPACE] = "KEYC_BACKSPACE",
    [KEYC_PRINT] = "KEYC_PRINT",
    [KEYC_F1] = "KEYC_F1",
    [KEYC_F2] = "KEYC_F2",
    [KEYC_F3] = "KEYC_F3",
    [KEYC_F4] = "KEYC_F4",
    [KEYC_F5] = "KEYC_F5",
    [KEYC_F6] = "KEYC_F6",
    [KEYC_F7] = "KEYC_F7",
    [KEYC_F8] = "KEYC_F8",
    [KEYC_F9] = "KEYC_F9",
    [KEYC_F10] = "KEYC_F10",
    [KEYC_F11] = "KEYC_F11",
    [KEYC_F12] = "KEYC_F12",
    [KEYC_LEFT] = "KEYC_LEFT",
    [KEYC_RIGHT] = "KEYC_RIGHT",
    [KEYC_UP] = "KEYC_UP",
    [KEYC_DOWN] = "KEYC_DOWN",
    [KEYC_SUPPR] = "KEYC_SUPPR",
    [KEYC_INSER] = "KEYC_INSER",
    [KEYC_BEGIN] = "KEYC_BEGIN",
    [KEYC_PAGE_UP] = "KEYC_PAGE_UP",
    [KEYC_PAGE_DOWN] = "KEYC_PAGE_DOWN",
    [KEYC_END] = "KEYC_END",
    [KEYC_PAUSE] = "KEYC_PAUSE",
    [KEYC_LWINDOW] = "KEYC_LWINDOW",
    [KEYC_RWINDOW] = "KEYC_RWINDOW",
    [KEYC_CAPSLOCK] = "KEYC_CAPSLOCK",
    [KEYC_NUMLOCK] = "KEYC_NUMLOCK",
    [KEYC_HOLD] = "KEYC_HOLD",
    [KEYC_ACUTE] = "KEYC_ACUTE",
    [KEYC_DIERE] = "KEYC_DIERE",
    [KEYC_CEDIL] = "KEYC_CEDIL",
};
#endif
// --------------------------------------------------------------------


typedef struct keyboard {
    int fd;
    keyc_t keys[KEYC_MAX];

    int init_kmode;
    struct termios init_termcfg;
    struct kbd_repeat init_krepeat;

    int evts_count;
    keybevt_t evts[MAXEVTS];
} keyboard_t;


static keyboard_t _KB = {
    .fd = -1,
    .evts_count = 0,
};


static int _push_evt(keybevt_t evt) {
    if (_KB.evts_count == MAXEVTS) {
        return -1;
    }
    _KB.evts[_KB.evts_count++] = evt;
    return 0;
}


int init_keyboard() {
    if (_KB.fd >= 0) {
        return -1;
    }

    _KB.evts_count = 0;

    _KB.fd = STDIN_FILENO;

    // Retrieve initial values
    ioctl(_KB.fd, KDGKBMODE, &_KB.init_kmode);
    _KB.init_krepeat = (struct kbd_repeat){
        .delay = 0,
        .period = 0
    };
    ioctl(_KB.fd, KDKBDREP, &_KB.init_krepeat);
    tcgetattr(_KB.fd, &_KB.init_termcfg);

    // Set keyboard mode
    ioctl(_KB.fd, KDSKBMODE, K_MEDIUMRAW);

    // Set keyboard delay
    struct kbd_repeat new_repeat = {
        .delay = 1,
        .period = 1
    };
    ioctl(_KB.fd, KDKBDREP, &new_repeat);

    // Set terminal mode
    tcgetattr(_KB.fd, &_KB.init_termcfg);
    struct termios ts = _KB.init_termcfg;
    ts.c_cc[VTIME] = 0;     // Read timeout
    ts.c_cc[VMIN] = 0;      // Non-blocking read, if nothing to read,
                            // read will return 0
    // Non-canonical mode, no echo and don't generate interruption
    // signals
    ts.c_lflag = 0;
    ts.c_iflag = 0;
    ts.c_cflag = CS8;
    tcsetattr(_KB.fd, TCSAFLUSH, &ts);

    memset(_KB.keys, 0, KEYC_MAX * sizeof(keyc_t));

    return 0;
}


void stop_keyboard() {
    if (_KB.fd < 0) {
        return;
    }
    ioctl(_KB.fd, KDSKBMODE, _KB.init_kmode);
    ioctl(_KB.fd, KDKBDREP, &_KB.init_krepeat);
    tcsetattr(_KB.fd, TCSAFLUSH, &_KB.init_termcfg);
    _KB.fd = -1;
}


// Convert a raw key (the key we read) to a key code in function of the
// current key table (SHIFT, ALTGR...).
static keyc_t _convert_raw_key(int table, unsigned char raw_key) {
    keyc_t index = KEYC_UNKNOWN;
    struct kbentry entry;
    int val, type ;

    entry.kb_table = table;
    entry.kb_index = raw_key & 0x7f;
    ioctl(_KB.fd, KDGKBENT, &entry);
    type = KTYP(entry.kb_value);
    val = KVAL(entry.kb_value);

#define MAPPING(_K, _KC) \
    case _K: index = _KC;  break;

#ifdef KEYB_DEBUG
    printf("KTYP=0x%x, KVAL=0x%x, kb_value=0x%x, raw_key=0x%x\n",
           type, val, entry.kb_value, raw_key & 0x7f);
#endif

    switch (type) {
      case KT_FN:
        if (val < 0xc) {
            index = KEYC_F1 + val;
        } else {
            switch (val) {
                MAPPING(22, KEYC_SUPPR)
                MAPPING(21, KEYC_INSER)
                MAPPING(20, KEYC_BEGIN)
                MAPPING(24, KEYC_PAGE_UP)
                MAPPING(25, KEYC_PAGE_DOWN)
                MAPPING(23, KEYC_END)
                MAPPING(29, KEYC_PAUSE)
            }
        }
        break;

      case KT_LATIN:
      case KT_LETTER:
        if (val == 0x7f) {
            index = KEYC_BACKSPACE;
        } else
        if (val == 0x1c) {
            index = KEYC_PRINT;
        } else {
            index = val;
        }
        break;

      case KT_DEAD:
        switch (val) {
            MAPPING(KVAL(K_DGRAVE), KEYC_GRAVE_ACCENT)
            MAPPING(KVAL(K_DACUTE), KEYC_ACUTE)
            MAPPING(KVAL(K_DCIRCM), KEYC_CIRCUMFLEX)
            MAPPING(KVAL(K_DTILDE), KEYC_TILDE)
            MAPPING(KVAL(K_DDIERE), KEYC_DIERE)
            MAPPING(KVAL(K_DCEDIL), KEYC_CEDIL)
        }
        break;

      case KT_SPEC:
        switch (val) {
            MAPPING(KVAL(K_ENTER), KEYC_ENTER)
            MAPPING(KVAL(K_CAPS), KEYC_CAPSLOCK)
            MAPPING(KVAL(K_NUM), KEYC_NUMLOCK)
            MAPPING(0x10, KEYC_LWINDOW)
            MAPPING(0x11, KEYC_RWINDOW)
            MAPPING(KVAL(K_HOLD), KEYC_HOLD)
            MAPPING(KVAL(K_COMPOSE), KEYC_PRINT)
        }
        break;

      case KT_CUR:
        switch (val) {
            MAPPING(KVAL(K_LEFT), KEYC_LEFT)
            MAPPING(KVAL(K_RIGHT), KEYC_RIGHT)
            MAPPING(KVAL(K_UP), KEYC_UP)
            MAPPING(KVAL(K_DOWN), KEYC_DOWN)
        }
        break;

      case KT_SHIFT:
        switch (val) {
            MAPPING(KVAL(K_CTRL), KEYC_CTRL)
            MAPPING(KVAL(K_ALT), KEYC_ALT)
            MAPPING(KVAL(K_ALTGR), KEYC_ALTGR)
            MAPPING(KVAL(K_SHIFT), KEYC_SHIFT)
        }
        break;
    }

    return index;
}


void update_keyboard() {
    _KB.evts_count = 0;

    unsigned char raw_key = 0;
    while (read(_KB.fd, &raw_key, 1) != 0) {
        bool pressed = !(raw_key & 0x80);
        int tab = K_NORMTAB;
        if (_KB.keys[KEYC_SHIFT]) {
            tab = K_SHIFTTAB;
        } else
        if (_KB.keys[KEYC_ALTGR]) {
            tab = K_ALTTAB;
        }
        keyc_t key = _convert_raw_key(tab, raw_key);
        if (key != KEYC_UNKNOWN) {
#ifdef KEYB_DEBUG
            printf("%s\n", key_names[key]);
#endif
            // Push event
            keybevt_t evt = (keybevt_t){
                .type = -1,
                .keyc = key
            };
            bool prev_state = _KB.keys[key];
            if (!prev_state && pressed) {
                evt.type = KEYBEVT_PRESSED;
            } else
            if (prev_state && pressed) {
                evt.type = KEYBEVT_HELD;
            } else
            if (prev_state && !pressed) {
                evt.type = KEYBEVT_RELEASED;
            }
            if (evt.type != -1) {
                _push_evt(evt);
            }

            _KB.keys[key] = pressed;
        }
#ifdef KEYB_DEBUG
        else if (!(raw_key & 0x80)) {
            printf("KEYC_UNKNOWN\n");
        }
#endif
    }
}


bool is_key_pressed(keyc_t key) {
    assert(key >= 0 && key < KEYC_MAX);
    return _KB.keys[key];
}


int poll_keyboard(keybevt_t* evt) {
    if (!_KB.evts_count) {
        return 0;
    }
    *evt = _KB.evts[--_KB.evts_count];
    return 1;
}

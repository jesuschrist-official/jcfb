#define DEBUG
#include "../src/keyboard.c"


int main(void) {
    if (init_keyboard() < 0) {
        fprintf(stderr, "Cannot initialize the keyboard\n");
    }

    int exit = 0;
    while (!exit) {
        update_keyboard();
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        usleep(1000);
    }
    stop_keyboard();

    return 0;
}

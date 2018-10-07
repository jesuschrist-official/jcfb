#define KEYB_DEBUG
#include "../src/keyboard.c"


static const char* evttypes_name[] = {
    "PRESSED", "HELD", "RELEASED"
};


int main(void) {
    if (init_keyboard() < 0) {
        fprintf(stderr, "Cannot initialize the keyboard\n");
    }

    int exit = 0;
    while (!exit) {
        // TODO disable the thing that is blocking causing some
        //      delay on the held event.
        update_keyboard();
        keybevt_t evt;
        while (poll_keyboard(&evt)) {
            printf("%s %s\n",
                   key_names[evt.keyc], evttypes_name[evt.type]);
        }
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        usleep(100000);
    }
    stop_keyboard();

    return 0;
}

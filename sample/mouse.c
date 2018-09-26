/*
 * Mouse sample
 */
#include <stdio.h>
#include <unistd.h>


#include "jcfb/mouse.h"


static const char* buttons[3] = {"LEFT", "RIGHT", "MIDDLE"};
static const char* state[2]   = {"PRESSED", "RELEASED"};


int main(int argc, char** argv) {
    if (init_mouse() < 0) {
        return 1;
    }

    while (1) {
        update_mouse();

        printf("%d %d\n", mouse_x(), mouse_y());
        mouseevt_t evt;
        while (poll_mouse(&evt)) {
            printf("%s %s at %d %d (speed %d %d)\n",
                   buttons[evt.button], state[evt.released],
                   evt.x, evt.y, evt.dx, evt.dy);
        }

        usleep(1000000);
    }

    stop_mouse();

    return 0;
}

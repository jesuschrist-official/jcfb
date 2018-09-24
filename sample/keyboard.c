/*
 * Sample
 *
 * This source file is the main sample showing how to use JCFB and which
 * capabilities the library has.
 */
#include <stdio.h>
#include <unistd.h>
#include "jcfb/jcfb.h"

#include <linux/keyboard.h>
#include <linux/kd.h>
#include <sys/ioctl.h>


int main(int argc, char** argv) {
    if (jcfb_start() < 0) {
        fprintf(stderr, "Cannot start JCFB\n");
        return 1;
    }

    int exit = 0;
    while (!exit) {
        if (is_key_pressed(KEYC_ESC)) {
            exit = 1;
        }
        jcfb_refresh(NULL);
    }

    jcfb_stop();
    printf("Gentle quit\n");
    return 0;
}

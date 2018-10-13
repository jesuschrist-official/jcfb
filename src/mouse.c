/*
 * Mouse handling module.
 *
 * The mouse is read from /dev/mouse0. This fd sends us PS/2 packets,
 * that is convenient.
 *
 * Will be ported to evdev soon (or not, depends... is that really
 * important ?).
 */
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>


#include "jcfb/mouse.h"
#include "jcfb/util.h"


#define MAXEVTS  1024


typedef struct mouse {
    int fd;

    bool left;
    bool right;
    bool middle;
    int x, y;
    int dx, dy;

    size_t evts_count;
    mouseevt_t evts[MAXEVTS];
    size_t poll_index;
} mouse_t;


static mouse_t _MS = {
    .fd = -1,
    .evts_count = 0,
};


// Fun part -----------------------------------------------------------
/* PS2 mouse frame */
__attribute__((packed))
struct mouse_frame {
    uint8_t left   : 1;
    uint8_t right  : 1;
    uint8_t middle : 1;
    uint8_t always : 1;
    uint8_t xsign  : 1;
    uint8_t ysign  : 1;
    uint8_t xoverf : 1; // Never seen true.
    uint8_t yoverf : 1;
    uint8_t x;
    uint8_t y;
};
static_assert(
    sizeof(struct mouse_frame) == 3,
    "struct mouse_frame must have a size of 3 bytes"
);


static int _push_evt(mousebtn_t btn, bool released) {
    if (_MS.evts_count == MAXEVTS) {
        return -1;
    }
    _MS.evts[_MS.evts_count++] = (mouseevt_t){
        .released = released,
        .button = btn,
        .x = _MS.x,
        .y = _MS.y,
        .dx = _MS.dx,
        .dy = _MS.dy
    };
    return 0;
}


static int _handle_frame(struct mouse_frame frame) {
    if (!frame.always) {
        fprintf(stderr, "Read invalid mouse frame\n");
        return -1;
    }
#ifdef MOUSE_DEBUG
    printf("MOUSE %x %x %x %x %x %x %x %x %x %x\n",
           frame.left, frame.right, frame.middle, frame.always,
           frame.xsign, frame.ysign, frame.xoverf, frame.yoverf,
           frame.x, frame.y);
#endif
    int dx = (frame.xsign) ? -(0xff - frame.x) : frame.x;
     // Inversed per default
    int dy = -((frame.ysign) ? -(0xff - frame.y) : frame.y);
    _MS.x = max(0, min(MOUSE_MAX_RANGE, _MS.x + dx));
    _MS.y = max(0, min(MOUSE_MAX_RANGE, _MS.y + dy));
    _MS.dx += dx;
    _MS.dy += dy;

    // Fire event if needed.
    if (frame.left && !_MS.left) {
        _push_evt(MOUSEBTN_LEFT, false);
    } else
    if (!frame.left && _MS.left) {
        _push_evt(MOUSEBTN_LEFT, true);
    }
    if (frame.right && !_MS.right) {
        _push_evt(MOUSEBTN_RIGHT, false);
    } else
    if (!frame.right && _MS.right) {
        _push_evt(MOUSEBTN_RIGHT, true);
    }
    if (frame.middle && !_MS.middle) {
        _push_evt(MOUSEBTN_MIDDLE, false);
    } else
    if (!frame.middle && _MS.middle) {
        _push_evt(MOUSEBTN_MIDDLE, true);
    }

    _MS.left = frame.left;
    _MS.right = frame.right;
    _MS.middle = frame.middle;

    return 0;
}


static bool _has_food() {
    fd_set fdset;
    struct timeval timeout;

    FD_ZERO(&fdset);
    FD_SET(_MS.fd, &fdset);
    memset(&timeout, 0, sizeof(struct timeval));
    return select(_MS.fd + 1, &fdset, NULL, NULL, &timeout) > 0;
}


static int _update_mouse() {
    // This is a choice...
    _MS.evts_count = 0;
    _MS.poll_index = 0;

    _MS.dx = 0;
    _MS.dy = 0;

    struct mouse_frame frame;
    while (_has_food()) {
        if (read(_MS.fd, &frame, sizeof(struct mouse_frame)) != sizeof(struct mouse_frame)) {
            return -1;
        }
        _handle_frame(frame);
    }

    return 0;
}


// User API -----------------------------------------------------------
int init_mouse() {
    if (_MS.fd >= 0) {
        return -1;
    }
    _MS.fd = open("/dev/input/mouse0", O_RDONLY);
    if (_MS.fd < 0) {
        fprintf(stderr, "Cannot open mouse file descriptor\n");
        return -1;
    }
    _MS.evts_count = 0;
    _MS.x = 0;
    _MS.y = 0;
    _MS.dx = 0;
    _MS.dy = 0;
    _MS.left = 0;
    _MS.right = 0;
    _MS.middle = 0;
    _MS.poll_index = 0;
    return 0;
}


void stop_mouse() {
    if (_MS.fd < 0) {
        return;
    }
    close(_MS.fd);
    _MS.fd = -1;
    _MS.evts_count = 0;
}


int mouse_x() {
    return _MS.x;
}


int mouse_y() {
    return _MS.y;
}


int mouse_x_speed() {
    return _MS.dx;
}


int mouse_y_speed() {
    return _MS.dy;
}


int update_mouse() {
    return _update_mouse();
}


int poll_mouse(mouseevt_t* evt) {
    if (_MS.poll_index == _MS.evts_count) {
        return 0;
    }
    *evt = _MS.evts[_MS.poll_index++];
    return 1;
}


// --------------------------------------------------------------------

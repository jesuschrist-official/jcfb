#ifndef _jcfb_util_h_
#define _jcfb_util_h_


#define min(_x, _y) (((_x) < (_y)) ? _x : _y)
#define max(_x, _y) (((_x) > (_y)) ? _x : _y)
#define clamp(_v, _x, _y) (max((_x), min((_v), (_y))))


#endif

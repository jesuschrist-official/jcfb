#include "jcfb/util.h"
#include "jcfb/primitive.h"


static bool _is_point_in_circle(int cx, int cy, int r, int x, int y) {
    int dx = x - cx;
    int dy = y - cy;
    return dx * dx + dy * dy - r * r <= -1;
}


#define PRIMITIVE_PIXEL_FUNC(_dst, _src) _dst = _src
#define PRIMITIVE_FUNC_SUFFIX
#include "primitive.inc.c"

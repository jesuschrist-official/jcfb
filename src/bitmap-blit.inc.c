#ifndef BLIT_PIXEL_FUNC
    #error "undefined BLIT_PIXEL_FUNC(dst, src) macro"
#endif

#ifndef BLIT_FUNC_SUFFIX
    #error "undefined BLIT_FUNC_SUFFIX"
#endif


#define __TCONCAT(x, y) x ## y
#define _TCONCAT(x, y) __TCONCAT(x, y)
#define FUNC(_name) _TCONCAT(_name, BLIT_FUNC_SUFFIX)


// In the following blit functions,
// x, y are the coordinates requested by the user,
// dx, dy are the current final coordinates on the `dst` bitmap and
// sx, sy are the current final coordinates on the `src` bitmap.
//
// user-coordinates are clamped to avoid copy in invalid memory. For exemple,
// if the `x` given by the user is negative, we ajust the source coordinate
// `sx` such that it corresponds to the shift given by the user (the distance
// from x to zero), as shown below, with x = -3, dx = 0, sx = 3 :
//
//   x  dx
//   |  |
//   |  v
//   |  ddddddddddd
//   v  ddddddddddd
//   sssssssddddddd
//   sssssssddddddd
//   sssssssddddddd
//      ddddddddddd
//      ddddddddddd
//      ^
//      |
//      sx
//
static void FUNC(_blit_row)(bitmap_t* dst, const bitmap_t* src,
                            int x, int dy, int sy)
{
    int dx = max(0, x);
    int sx = max(0, -x);
    int max_size = min(src->w - sx, dst->w - dx);
    if (max_size < 0) {
        return;
    }
    pixel_t* dest_addr = dst->mem + dy * dst->w + dx;
    const pixel_t* src_addr = src->mem + sy * src->w + sx;
    for (size_t i = 0; i < max_size; i++) {
        BLIT_PIXEL_FUNC(dest_addr[i], src_addr[i]);
    }
}


static void FUNC(_fast_blit)(bitmap_t* dst, const bitmap_t* src,
                             int x, int y)
{
    // If `y` is offscreen, we start to copy `src` from the `-y` row to
    // `dst` on the first row.
    int dy = max(0, y);
    int sy = max(0, -y);
    for (; dy < dst->h && sy < src->h; dy++, sy++) {
        FUNC(_blit_row)(dst, src, x, dy, sy);
    }
}


static void FUNC(_slow_blit)(bitmap_t* dst, const bitmap_t* src,
                             int x, int y)
{
    int dy = max(0, y);
    int sy = max(0, -dy);
    for (; dy < dst->h && sy < src->h; dy++, sy++) {
        _blit_row(dst, src, x, dy, sy);
        _convert_row(dst, x, dy, src->fmt);
    }
}


void FUNC(bitmap_blit)(bitmap_t* dst, const bitmap_t* src, int x, int y) {
    if (dst->fmt == src->fmt) {
        FUNC(_fast_blit)(dst, src, x, y);
    } else {
        FUNC(_slow_blit)(dst, src, x, y);
    }
}


static void FUNC(_blit_scaled_row)(bitmap_t* dst, const bitmap_t* src,
                                   int x, int y, int w, int sy)
{
    int dx = max(0, x);
    float sx = max(0, -x);
    float xratio = src->w / (float)w;
    for (; dx < min(x + w, dst->w) && sx * xratio < src->w; dx++, sx++)
    {
        size_t src_off = sy * src->w + sx * xratio;
        BLIT_PIXEL_FUNC(dst->mem[y * dst->w + dx], src->mem[src_off]);
    }
}


void FUNC(bitmap_scaled_blit)(bitmap_t* dst, const bitmap_t* src,
                              int x, int y, int w, int h)
{
    int dy = max(0, y);
    float sy = max(0, -y);
    float yratio = src->h / (float)h;
    for (; dy < min(y + h, dst->h) && sy * yratio < src->h; dy++, sy++)
    {
        FUNC(_blit_scaled_row)(dst, src, x, dy, w, sy * yratio);
    }
}


static void FUNC(_blit_scaled_region_row)(bitmap_t* dst, const bitmap_t* src,
                                          int dst_x, int dst_y, int dst_w,
                                          int src_x, int src_y,
                                          int src_w)
{
    int dx = max(0, dst_x);
    int sx = max(0, -dst_x);

    int dx_max = min(dst_x + dst_w, dst->w);
    int sx_max = min(src_x + src_w, src->w);

    float xratio = src_w / (float)dst_w;


    for (; dx < dx_max && src_x + sx * xratio < sx_max; dx++, sx++) {
        size_t dst_off = dst_y * dst->w + dx;
        size_t src_off = (size_t)(src_y * src->w + src_x + sx * xratio);
        BLIT_PIXEL_FUNC(dst->mem[dst_off], src->mem[src_off]);
    }
}


void FUNC(bitmap_scaled_region_blit)(bitmap_t* dst, const bitmap_t* src,
                                     int src_x, int src_y, int src_w,
                                     int src_h,
                                     int dst_x, int dst_y, int dst_w,
                                     int dst_h)
{
    int dy = max(0, dst_y);
    float sy = max(0, -dst_y);

    int dy_max = min(dst_y + dst_h, dst->h);
    float sy_max = min(src_y + src_h, src->h);

    float yratio = src_h / (float)dst_h;

    for (; dy < dy_max && src_y + sy * yratio < sy_max; dy++, sy++) {
        FUNC(_blit_scaled_region_row)(
            dst, src,
            dst_x, dy, dst_w,
            src_x, src_y + sy * yratio, src_w
        );
    }
}


static void FUNC(_blit_row_hflip)(bitmap_t* dst, const bitmap_t* src,
                                  int x, int dy, int sy)
{
    int dx = max(0, x);
    int sx = max(0, -x);
    int max_size = min(src->w - sx, dst->w - dx);
    if (max_size < 0) {
        return;
    }
    pixel_t* dest_addr = dst->mem + dy * dst->w + dx;
    const pixel_t* src_addr = src->mem + sy * src->w + src->w - sx - 1;
    for (size_t i = 0; i < max_size; i++) {
        BLIT_PIXEL_FUNC(dest_addr[i], src_addr[-i]);
    }
}


static void FUNC(_fast_blit_hflip)(bitmap_t* dst, const bitmap_t* src,
                                   int x, int y)
{
    // If `y` is offscreen, we start to copy `src` from the `-y` row to
    // `dst` on the first row.
    int dy = max(0, y);
    int sy = max(0, -y);
    for (; dy < dst->h && sy < src->h; dy++, sy++) {
        FUNC(_blit_row_hflip)(dst, src, x, dy, sy);
    }
}


static void FUNC(_slow_blit_hflip)(bitmap_t* dst, const bitmap_t* src,
                                   int x, int y)
{
    int dy = max(0, y);
    int sy = max(0, -dy);
    for (; dy < dst->h && sy < src->h; dy++, sy++) {
        _blit_row_hflip(dst, src, x, dy, sy);
        _convert_row(dst, x, dy, src->fmt);
    }
}


void FUNC(bitmap_blit_hflip)(bitmap_t* dst, const bitmap_t* src, int x, int y)
{
    if (dst->fmt == src->fmt) {
        FUNC(_fast_blit_hflip)(dst, src, x, y);
    } else {
        FUNC(_slow_blit_hflip)(dst, src, x, y);
    }
}

#undef BLIT_FUNC_SUFFIX
#undef BLIT_PIXEL_FUNC
#undef __TCONCAT
#undef _TCONCAT
#undef FUNC

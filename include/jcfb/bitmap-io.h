#ifndef _jcfb_bitmap_io_h_
#define _jcfb_bitmap_io_h_


#include "jcfb/bitmap.h"


/*
 * Load a bitmap from file `path`.
 */
int bitmap_load(bitmap_t* bmp, const char* path);


#if 0
/*
 * Save a bitmap in file `path`.
 */
int bitmap_save(const bitmap_t* bmp, const char* path);
#endif


#endif

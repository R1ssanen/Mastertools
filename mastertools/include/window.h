#ifndef MASTERTOOLS_WINDOW_H_
#define MASTERTOOLS_WINDOW_H_

#include "types.h"
#include "utility/mstring.h"

typedef struct mt_window mt_window;
struct mt_allocator;

enum
{
    MT_WINDOW_NONE,
    MT_WINDOW_FULLSCREEN,
    MT_WINDOW_RESIZABLE,
};

mt_window *mt_window_create(struct mt_allocator *alloc, char *title, int w, int h, int flags);

void mt_window_free(mt_window *window);

void mt_window_resize(mt_window *window, int w, int h);

void mt_window_render(mt_window *window, int *pixels, int w);

#endif
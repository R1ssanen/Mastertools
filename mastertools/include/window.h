#ifndef MASTERTOOLS_WINDOW_H_
#define MASTERTOOLS_WINDOW_H_

#include "types.h"
#include "utility/mtstring.h"

typedef struct mt_window mt_window;

enum mt_window_flag
{
    MT_WINDOW_NONE,
    MT_WINDOW_FULLSCREEN,
    MT_WINDOW_RESIZABLE,
};

mt_window *mt_window_create(char *title, int w, int h, enum mt_window_flag flags);

void mt_window_free(mt_window *window);

void mt_window_resize(mt_window *window, int w, int h);

void mt_window_render(mt_window *window, int *pixels, int w);

#endif
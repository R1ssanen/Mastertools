#ifndef MASTERTOOLS_TEXT_H_
#define MASTERTOOLS_TEXT_H_

#include "utility/mtstring.h"

struct mt_texture;

void mt_render_text_2d_default(struct mt_texture *pixels, mt_string_view text, int x, int y, int w, int h, int color);

#endif
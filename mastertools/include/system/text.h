#ifndef MASTERTOOLS_TEXT_H_
#define MASTERTOOLS_TEXT_H_

#include "utility/mtstring.h"

struct rohan_shader_spec;

void mt_render_text_2d(const struct rohan_shader_spec *spec, void *instance, size_t target_pitch, size_t target_stride,
                       mt_string_view text, int x, int y, int w, int h);

void mt_render_text_2d_default(int *pixels, int width, mt_string_view text, int x, int y, int w, int h, int color);

#endif
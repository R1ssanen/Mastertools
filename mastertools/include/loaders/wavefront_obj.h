#ifndef MASTERTOOLS_WAVEFRONT_OBJ_H_
#define MASTERTOOLS_WAVEFRONT_OBJ_H_

#include <stdio.h>

#include "types.h"
#include "utility/mstring.h"

struct mt_entity *mt_load_wavefront_obj(mt_string_view path);

#endif
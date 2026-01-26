#ifndef MASTERTOOLS_WAVEFRONT_OBJ_H_
#define MASTERTOOLS_WAVEFRONT_OBJ_H_

#include "types.h"
#include "utility/mtstring.h"

struct mt_entity *mt_load_wavefront_obj(mt_string_view path);

#endif
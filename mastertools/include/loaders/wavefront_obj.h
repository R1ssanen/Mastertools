#ifndef MASTERTOOLS_WAVEFRONT_OBJ_H_
#define MASTERTOOLS_WAVEFRONT_OBJ_H_

#include <stdio.h>

#include "allocator.h"
#include "scene/entity.h"
#include "types.h"

static inline mt_entity *load_entity_wavefront_obj(mt_allocator *alloc, const char *path)
{
    mt_entity *entity = allocate(alloc, sizeof(mt_entity));
    return entity;
}

#endif
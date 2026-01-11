#ifndef MASTERTOOLS_ARRAY_H_
#define MASTERTOOLS_ARRAY_H_

#include "types.h"

typedef struct mt_array mt_array;
struct mt_array
{
    void *data;
    size_t stride;
    size_t size;
    size_t capacity;
};

mt_array mt_array_create_reserved(size_t stride, size_t count);

static inline mt_array mt_array_create(size_t stride)
{
    return mt_array_create_reserved(stride, 1);
}

void mt_array_free(mt_array *array);

void mt_array_resize(mt_array *array, size_t new_capacity);

void mt_array_push(mt_array *restrict array, const void *restrict value);

void mt_array_pop(mt_array *restrict array);

#define mt_array_get(array, type, index) ((type *)((array)->data))[index]

#define mt_array_foreach(array, type, iterator)                                                                        \
    for (type *iterator = (type *)((array)->data); iterator < (type *)((array)->data) + (array)->size; ++iterator)

#endif
#ifndef MASTERTOOLS_ARRAY_H_
#define MASTERTOOLS_ARRAY_H_

#include "types.h"

#define mt_array_of(type_hint) mt_array

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

// returns pointer to pushed element, if value == NULL reserves space only
void *mt_array_push(mt_array *array, const void *value);

void mt_array_pop(mt_array *array);

#define mt_array_ptr(array, type, index) (((type *)((array)->data)) + index)

#define mt_array_get(array, type, index) (*mt_array_ptr(array, type, index))

#define mt_array_foreach(array, type, iterator)                                                                        \
    for (type *iterator = (type *)((array)->data); iterator < (type *)((array)->data) + (array)->size; ++iterator)

#endif
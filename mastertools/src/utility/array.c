#include "utility/array.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "types.h"

mt_array mt_array_create_reserved(size_t stride, size_t count)
{
    if (count == 0)
    {
        count = 1;
    }

    void *data = malloc(stride * count);
    if (!data)
    {
        LFATAL("Memory allocation for dynamic array failed");
    }

    return (mt_array){.data = data, .capacity = count, .size = 0, .stride = stride};
}

void mt_array_free(mt_array *array)
{
    free(array->data);

#if defined(MT_SANITIZE_FREE)
    memset(array, 0, sizeof(*array));
#endif
}

void mt_array_resize(mt_array *array, size_t new_capacity)
{
    if (new_capacity == 0)
    {
        new_capacity = 1;
    }
    else if (new_capacity == array->capacity)
    {
        return;
    }

    void *new_data = realloc(array->data, new_capacity * array->stride);
    if (!new_data)
    {
        LFATAL("Memory reallocation for dynamic array failed");
    }

    array->data = new_data;
    array->capacity = new_capacity;

    if (new_capacity < array->size) // shrink to fit
    {
        array->size = new_capacity;
    }
}

void mt_array_push(mt_array *restrict array, const void *restrict value)
{
    if (array->size == array->capacity)
    {
        size_t new_capacity = array->capacity + (array->capacity >> 1);
        if (new_capacity < array->capacity + 1)
        {
            new_capacity = array->capacity + 1;
        }

        mt_array_resize(array, new_capacity);
    }

    memcpy((char *)array->data + array->size * array->stride, value, array->stride);
    array->size += 1;
}

void mt_array_pop(mt_array *restrict array)
{
    if (array->size > 0)
    {
        array->size -= 1;
    }
}

#include "utility/array.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "math/common.h"
#include "mtassert.h"
#include "types.h"

mt_array mt_array_create_reserved(size_t stride, size_t count)
{
    MT_ASSERT(stride != 0, "Array stride cannot be 0");

    void *data = malloc(MT_MAX(count, 1) * stride);
    MT_ASSERT(data != NULL, "Memory allocation for dynamic array failed");

    return (mt_array){.data = data, .stride = stride, .capacity = count, .size = 0};
}

void mt_array_free(mt_array *array)
{
    if (!array)
    {
        return;
    }

    if (array->data)
    {
        free(array->data);
    }
}

void mt_array_resize(mt_array *array, size_t new_capacity)
{
    if (new_capacity == array->capacity)
    {
        return;
    }
    else if (new_capacity == 0)
    {
        new_capacity = 1;
    }

    void *resized_data = realloc(array->data, new_capacity * array->stride);
    MT_ASSERT(resized_data != NULL, "Memory reallocation for dynamic array failed");

    array->data = resized_data;
    array->capacity = new_capacity;

    if (new_capacity < array->size) // shrink to fit
    {
        array->size = new_capacity;
    }
}

void *mt_array_push(mt_array *array, const void *value)
{
    if (array->size == array->capacity)
    {
        size_t new_capacity = array->capacity + (array->capacity >> 1);
        mt_array_resize(array, MT_MAX(new_capacity, array->capacity + 1));
    }

    void *element = (uint8_t *)array->data + array->size * array->stride;
    array->size += 1;

    if (value)
    {
        memcpy(element, value, array->stride);
    }

    return element;
}

void mt_array_pop(mt_array *array)
{
    if (array->size > 0)
    {
        array->size -= 1;
    }
}

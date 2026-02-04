#ifndef MASTERTOOLS_HASH_H_
#define MASTERTOOLS_HASH_H_

#include "mtstring.h"
#include "types.h"

static inline size_t mt_hash_fnv_1a_64(mt_string_view view)
{
    size_t hash = 0xcbf29ce484222325;

    for (size_t i = 0; i < view.len; ++i)
    {
        hash ^= view.str[i];
        hash *= 0x00000100000001b3;
    }

    return hash;
}

static inline uint mt_hash_fnv_1a_32(mt_string_view view)
{
    uint hash = 0x811c9dc5;

    for (size_t i = 0; i < view.len; ++i)
    {
        hash ^= view.str[i];
        hash *= 0x01000193;
    }

    return hash;
}

static inline int mt_hash_murmur3(int x)
{
    x ^= x >> 16;
    x *= 0x85ebca6b;
    x ^= x >> 13;
    x *= 0xc2b2ae35;
    x ^= x >> 16;
    return x;
}

#endif
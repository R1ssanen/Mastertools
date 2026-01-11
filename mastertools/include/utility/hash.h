#ifndef MASTERTOOLS_HASH_H_
#define MASTERTOOLS_HASH_H_

#include "mstring.h"
#include "types.h"

static inline size_t mt_hash_fnv_1a(const char *str)
{
    size_t hash = 0xcbf29ce484222325;

    for (size_t i = 0; i < strlen(str); ++i)
    {
        hash ^= (byte)str[i];
        hash *= 0x00000100000001b3;
    }

    return hash;
}

#endif
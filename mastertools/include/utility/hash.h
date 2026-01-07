#ifndef MASTERTOOLS_HASH_H_
#define MASTERTOOLS_HASH_H_

#include "mstring.h"
#include "types.h"

static inline int mt_hash_fnv_1a(mstring str)
{
    size_t hash = 0xcbf29ce484222325;

    for (int i = 0; i < str.len; ++i)
    {
        hash ^= (unsigned char)str.str[i];
        hash *= 0x00000100000001b3;
    }

    return hash;
}

#endif
#ifndef MASTERTOOLS_LIBRARY_H_
#define MASTERTOOLS_LIBRARY_H_

#include "types.h"
#include "utility/mstring.h"

typedef struct mt_library mt_library;
struct mt_library
{
    mstring path;
    void *handle;
};

bool load_library(mstring path, mt_library *lib);

void *load_library_symbol(const mt_library *lib, mstring name);

bool free_library(mt_library *lib);

#endif
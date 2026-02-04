#ifndef MASTERTOOLS_LIBRARY_H_
#define MASTERTOOLS_LIBRARY_H_

#include "types.h"
#include "utility/mtstring.h"

typedef struct mt_library mt_library;
struct mt_library
{
    mt_string path;
    void *handle;
};

bool mt_library_load(mt_string_view path, mt_library *lib);

void *mt_library_load_symbol(const mt_library *lib, mt_string_view name);

void mt_library_free(mt_library *lib);

#endif
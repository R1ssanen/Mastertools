#include "system/library.h"
#include "logging.h"
#include "types.h"
#include "utility/mstring.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>

bool load_library(mstring path, mt_library *lib)
{
    lib->path = mt_mstring_copy(path);
    lib->handle = (void *)LoadLibraryA((LPCSTR)path.str);
    if (!lib->handle)
    {
        LERROR("Could not open dynamic library file '%s'", path.str);
        return false;
    }

    return true;
}

void *load_library_symbol(const mt_library *lib, mstring name)
{
    FARPROC symbol = GetProcAddress((HMODULE)lib->handle, (LPCSTR)name.str);
    if (!symbol)
    {
        LERROR("Could not load dynamic library symbol '%s' from '%s'", name.str, lib->path.str);
        return NULL;
    }

    return (void *)symbol;
}

bool free_library(mt_library *lib)
{
    if (FreeLibrary((HMODULE)lib->handle) == 0)
    {
        LERROR("Could not free dynamic library '%s'", lib->path.str);
        return false;
    }

    mt_mstring_free(lib->path);
    return true;
}

#endif
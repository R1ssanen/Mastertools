#include "system/library.h"
#include "logging.h"
#include "types.h"
#include "utility/mstring.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>

bool mt_library_load(mt_string_view path, mt_library *lib)
{
    lib->handle = (void *)LoadLibraryA((LPCSTR)path.str);
    if (!lib->handle)
    {
        LERROR("Could not open dynamic library file '%s'", path.str);
        return false;
    }

    lib->path = mt_string_copy_view(path);
    return true;
}

void *mt_library_load_symbol(const mt_library *lib, mt_string_view name)
{
    FARPROC symbol = GetProcAddress((HMODULE)lib->handle, (LPCSTR)name.str);
    if (!symbol)
    {
        LERROR("Could not load dynamic library symbol '%s' from '%s'", name.str, lib->path.str);
        return NULL;
    }

    return (void *)symbol;
}

bool mt_library_free(mt_library *lib)
{
    if (FreeLibrary((HMODULE)lib->handle) == 0)
    {
        LERROR("Could not free dynamic library '%s'", lib->path.str);
        return false;
    }

    mt_string_free(&lib->path);

#if defined(MT_SANITIZE_FREE)
    memset(lib, 0, sizeof(*lib));
#endif

    return true;
}

#endif
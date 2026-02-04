#if defined(_WIN32) || defined(__CYGWIN__)

#include "system/library.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "logging.h"
#include "types.h"
#include "utility/mtstring.h"
#include "utility/zero_init.h"

bool mt_library_load(mt_string_view path, mt_library *lib)
{
    MT_ZERO_INIT(lib);

    lib->handle = (void *)LoadLibraryA((LPCSTR)path.str);
    if (!lib->handle)
    {
        LERROR("Could not open dynamic library file '%s'", path.str);
        goto fail;
    }

    lib->path = mt_string_copy_view(path);
    return true;

fail:
    mt_library_free(lib);
    return false;
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

void mt_library_free(mt_library *lib)
{
    if (!lib)
    {
        return;
    }

    if (lib->handle)
    {
        FreeLibrary((HMODULE)lib->handle);
    }

    mt_string_free(&lib->path);

    MT_ZERO_INIT(lib);
}

#endif
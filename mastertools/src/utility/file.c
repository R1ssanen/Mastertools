#include "utility/file.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "types.h"

bool mt_read_file(mstring path, byte **buffer, size_t *count)
{
    FILE *file = fopen(path.str, "rb");
    if (!file)
    {
        LERROR("Failed to open file '%s': %s", path.str, strerror(errno));
        return false;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    rewind(file);

    if (length == -1)
    {
        LERROR("Failed reading the length of file '%s': %s", path.str, strerror(errno));
        return false;
    }

    *count = length;
    if (length == 0)
    {
        LWARN("File '%s' is empty; meant to do that?", path.str);
        *buffer = NULL;
        return true;
    }

    *buffer = malloc(length * sizeof(char));
    fread((void *)*buffer, sizeof(char), length, file);
    fclose(file);
    return true;
}

mstring mt_get_file_name(mstring path)
{
    const char *a = strrchr(path.str, '/');
    const char *b = strrchr(path.str, '\\');
    const char *found = a > b ? a : b;

    if (!found)
    {
        return path;
    }

    return mt_mstring_copy_raw(found + 1);
}

mstring mt_get_file_extension(mstring path)
{
    return path;
}

mstring mt_get_directory(mstring path)
{
    return path;
}

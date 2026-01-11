#include "utility/file.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "types.h"

bool mt_file_read(mt_string_view path, byte **buffer, size_t *count)
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

bool mt_file_split_directory_name(mt_string_view path, mt_string *directory, mt_string *name)
{
    char last = path.str[path.len - 1];
    if ((last == '/') || (last == '\\'))
    {
        if (directory)
        {
            *directory = mt_string_copy_view(path);
        }

        return false;
    }

    for (size_t i = path.len - 1; i > 0; --i)
    {
        char left = path.str[i - 1];
        if ((left == '/') || (left == '\\'))
        {
            if (name)
            {
                *name = mt_string_copy_range_view(path, i, path.len - i);
            }
            if (directory)
            {
                *directory = mt_string_copy_range_view(path, 0, i);
            }

            return true;
        }
    }

    if (name)
    {
        *name = mt_string_copy_view(path);
    }
    if (directory)
    {
        *directory = mt_string_create(0);
    }

    return true;
}

bool mt_file_get_extension(mt_string_view path, mt_string *extension)
{
    for (size_t i = path.len - 1; i > 0; --i)
    {
        char curr = path.str[i];
        if ((curr == '/') || (curr == '\\'))
        {
            return false;
        }
        else if (curr != '.')
        {
            continue;
        }

        char left = path.str[i - 1];
        if ((left == '/') || (left == '\\'))
        {
            return false;
        }

        *extension = mt_string_copy_range_view(path, i, path.len - i);
        return true;
    }

    return false;
}
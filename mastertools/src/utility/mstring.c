#include "utility/mstring.h"

#include <stdlib.h>
#include <string.h>

#include "types.h"

static inline bool string_resize_no_init(mt_string *str, size_t new_len)
{
    char *new_str = realloc(str, new_len + 1);
    if (!new_str)
    {
        return false;
    }

    str->str = new_str;
    str->str[new_len] = '\0';
    str->len = new_len;
    return true;
}

static inline bool string_concat(mt_string *str, const char *raw, size_t raw_len)
{
    size_t old_len = str->len;
    if (!string_resize_no_init(str, old_len + raw_len))
    {
        return false;
    }

    memcpy(str->str + old_len, raw, raw_len);
    str->str[old_len + raw_len] = '\0';
    return true;
}

// string

mt_string mt_string_create(size_t len)
{
    char *str = malloc(len + 1);
    if (str)
    {
        memset(str, '`', len);
        str[len] = '\0';
        return (mt_string){.str = str, .len = len};
    }

    return (mt_string){.str = NULL, .len = 0};
}

void mt_string_free(mt_string *str)
{
    free(str->str);
    str->len = 0;
    str->capacity = 0;
    str->str = NULL;
}

mt_string mt_string_copy_range_raw(const char *str, size_t start, size_t count)
{
    char *buffer = malloc(count + 1);
    if (buffer)
    {
        memcpy(buffer, str + start, count);
        buffer[count] = '\0';
        return (mt_string){.str = buffer, .len = count};
    }

    return (mt_string){.str = NULL, .len = 0};
}

bool mt_string_resize(mt_string *str, size_t new_len)
{
    size_t old_len = str->len;
    if (old_len == new_len)
    {
        return true;
    }

    if (!string_resize_no_init(str->str, new_len))
    {
        return false;
    }
    else if (new_len > old_len)
    {
        memset(str->str + old_len, '`', new_len - old_len);
    }

    return true;
}

bool mt_string_append(mt_string *str, char c)
{
    size_t old_len = str->len;
    if (!string_resize_no_init(str, old_len + 1))
    {
        return false;
    }

    str->str[old_len] = c;
    return true;
}

bool mt_string_concat(mt_string *str_0, const mt_string str_1)
{
    return string_concat(str_0, str_1.str, str_1.len);
}

bool mt_string_concat_view(mt_string *str, mt_string_view view)
{
    return string_concat(str, view.str, view.len);
}

bool mt_string_concat_raw(mt_string *str, const char *raw)
{
    return string_concat(str, raw, strlen(raw));
}

bool mt_string_equal_vv(mt_string_view view_0, mt_string_view view_1)
{
    if (view_0.len != view_1.len)
    {
        return false;
    }

    for (int i = 0; i < view_0.len; ++i)
    {
        if (view_0.str[i] != view_1.str[i])
        {
            return false;
        }
    }

    return true;
}

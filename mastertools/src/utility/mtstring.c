#include "utility/mtstring.h"

#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "mtassert.h"
#include "types.h"
#include "utility/zero_init.h"

static inline void string_resize_no_init(mt_string *str, size_t new_len)
{
    char *new_str = realloc(str->str, new_len + 1);
    MT_ASSERT(new_str != NULL, "Memory reallocation for string failed");

    str->str = new_str;
    str->str[new_len] = '\0';
    str->len = new_len;
}

static inline void string_concat(mt_string *str, const char *raw, size_t raw_len)
{
    size_t old_len = str->len;
    string_resize_no_init(str, old_len + raw_len);
    memcpy(str->str + old_len, raw, raw_len);
    str->str[old_len + raw_len] = '\0';
}

// string

mt_string mt_string_create(size_t len)
{
    char *str = malloc(len + 1);
    MT_ASSERT(str != NULL, "Memory allocation for string failed");

    memset(str, '`', len);
    str[len] = '\0';
    return (mt_string){.str = str, .len = len};
}

void mt_string_free(mt_string *str)
{
    if (!str)
    {
        return;
    }

    if (str->str)
    {
        free(str->str);
    }

    MT_ZERO_INIT(str);
}

mt_string mt_string_copy_range_raw(const char *str, size_t start, size_t count)
{
    char *buffer = malloc(count + 1);
    MT_ASSERT(buffer != NULL, "Memory allocation for string failed");

    memcpy(buffer, str + start, count);
    buffer[count] = '\0';
    return (mt_string){.str = buffer, .len = count};
}

void mt_string_resize(mt_string *str, size_t new_len)
{
    size_t old_len = str->len;
    if (old_len == new_len)
    {
        return;
    }

    string_resize_no_init(str, new_len);

    if (new_len > old_len)
    {
        memset(str->str + old_len, '`', new_len - old_len);
    }
}

void mt_string_append(mt_string *str, char c)
{
    size_t old_len = str->len;
    string_resize_no_init(str, old_len + 1);
    str->str[old_len] = c;
}

void mt_string_concat(mt_string *str_0, const mt_string str_1)
{
    string_concat(str_0, str_1.str, str_1.len);
}

void mt_string_concat_view(mt_string *str, mt_string_view view)
{
    string_concat(str, view.str, view.len);
}

void mt_string_concat_raw(mt_string *str, const char *raw)
{
    string_concat(str, raw, strlen(raw));
}

bool mt_string_equal_vv(mt_string_view view_0, mt_string_view view_1)
{
    if (view_0.len != view_1.len)
    {
        return false;
    }

    for (size_t i = 0; i < view_0.len; ++i)
    {
        if (view_0.str[i] != view_1.str[i])
        {
            return false;
        }
    }

    return true;
}

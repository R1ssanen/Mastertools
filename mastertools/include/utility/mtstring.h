#ifndef MASTERTOOLS_MSTRING_H_
#define MASTERTOOLS_MSTRING_H_

#include <string.h>

#include "types.h"

// owned
typedef struct mt_string mt_string;
struct mt_string
{
    char *str;
    size_t len;
};

// non-owned
typedef struct mt_string_view mt_string_view;
struct mt_string_view
{
    const char *str;
    size_t len;
};

mt_string mt_string_create(size_t len);

void mt_string_free(mt_string *str);

// copies the slice ( str[start], str[start + count] )
mt_string mt_string_copy_range_raw(const char *str, size_t start, size_t count);

static inline mt_string mt_string_copy(const mt_string str)
{
    return mt_string_copy_range_raw(str.str, 0, str.len);
}

static inline mt_string mt_string_copy_view(mt_string_view view)
{
    return mt_string_copy_range_raw(view.str, 0, view.len);
}

static inline mt_string mt_string_copy_raw(const char *str)
{
    return mt_string_copy_range_raw(str, 0, strlen(str));
}

static inline mt_string mt_string_copy_range(const mt_string str, size_t start, size_t count)
{
    return mt_string_copy_range_raw(str.str, start, count);
}

static inline mt_string mt_string_copy_range_view(mt_string_view view, size_t start, size_t count)
{
    return mt_string_copy_range_raw(view.str, start, count);
}

void mt_string_resize(mt_string *str, size_t new_len);

void mt_string_append(mt_string *str, char c);

void mt_string_concat(mt_string *str_0, const mt_string str_1);

void mt_string_concat_view(mt_string *str, mt_string_view view);

void mt_string_concat_raw(mt_string *str, const char *raw);

// refers to slice ( str[start], str[start + count] )
static inline mt_string_view mt_string_refer_range_raw(const char *str, size_t start, size_t count)
{
    return (mt_string_view){.str = str + start, .len = count};
}

static inline mt_string_view mt_string_refer(const mt_string str)
{
    return mt_string_refer_range_raw(str.str, 0, str.len);
}

static inline mt_string_view mt_string_refer_raw(const char *str)
{
    return mt_string_refer_range_raw(str, 0, strlen(str));
}

static inline mt_string_view mt_string_refer_range(const mt_string str, size_t start, size_t count)
{
    return mt_string_refer_range_raw(str.str, start, count);
}

// comparisons

bool mt_string_equal_vv(mt_string_view view_0, mt_string_view view_1);

static inline bool mt_string_equal_ss(const mt_string str_0, const mt_string str_1)
{
    return mt_string_equal_vv(mt_string_refer(str_0), mt_string_refer(str_1));
}

static inline bool mt_string_equal_vs(mt_string_view view, const mt_string str)
{
    return mt_string_equal_vv(view, mt_string_refer(str));
}

static inline bool mt_string_equal_sr(const mt_string str, const char *raw)
{
    return mt_string_equal_vv(mt_string_refer(str), mt_string_refer_raw(raw));
}

static inline bool mt_string_equal_vr(mt_string_view view, const char *raw)
{
    return mt_string_equal_vv(view, mt_string_refer_raw(raw));
}

#endif

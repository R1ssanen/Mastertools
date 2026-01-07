#include "utility/mstring.h"

#include <stdlib.h>
#include <string.h>

#include "types.h"

mstring mt_mstring_copy_raw(const char *str)
{
    int len = strlen(str);
    mstring b;
    b.str = malloc((len + 1) * sizeof(char));
    b.len = len;
    memmove(b.str, str, len + 1);
    return b;
}

mstring mt_mstring_copy(mstring a)
{
    mstring b;
    b.len = a.len;
    b.str = malloc((b.len + 1) * sizeof(char));
    memmove(b.str, a.str, b.len + 1);
    return b;
}

void mt_mstring_free(mstring a)
{
    free(a.str);
}

mstring mt_mstring_resize(mstring a, size_t new_len)
{
    a.str = realloc(a.str, new_len * sizeof(char));
    a.len = new_len;
    mt_mstring_terminate(a);
    return a;
}

mstring mt_mstring_concat(mstring a, mstring b)
{
    size_t len_old_a = a.len;
    a = mt_mstring_resize(a, len_old_a + b.len + 1);
    memmove(a.str + len_old_a + 1, b.str, b.len + 1);
    return a;
}

mstring mt_mstring_terminate(mstring a)
{
    a.str[a.len] = '\0';
    return a;
}

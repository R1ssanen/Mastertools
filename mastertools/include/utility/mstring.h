#ifndef MASTERTOOLS_MSTRING_H_
#define MASTERTOOLS_MSTRING_H_

#include "types.h"

typedef struct mstring mstring;
struct mstring
{
    char *str;
    size_t len;
};

mstring mt_mstring_copy_raw(const char *a);

mstring mt_mstring_copy(mstring a);

void mt_mstring_free(mstring a);

mstring mt_mstring_resize(mstring a, size_t new_len);

mstring mt_mstring_concat(mstring a, mstring b);

mstring mt_mstring_terminate(mstring a);

#endif

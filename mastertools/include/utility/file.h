#ifndef MASTERTOOLS_FILE_H_
#define MASTERTOOLS_FILE_H_

#include "mstring.h"
#include "types.h"

bool mt_read_file(mstring path, byte **buffer, size_t *count);

mstring mt_get_file_name(mstring path);

mstring mt_get_file_extension(mstring path);

mstring mt_get_directory(mstring path);

#endif
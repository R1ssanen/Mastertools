#ifndef MASTERTOOLS_FILE_H_
#define MASTERTOOLS_FILE_H_

#include "mstring.h"
#include "types.h"

bool mt_file_read(mt_string_view path, byte **buffer, size_t *count);

bool mt_file_split_directory_name(mt_string_view path, mt_string *directory, mt_string *filename);

bool mt_file_get_extension(mt_string_view path, mt_string *extension);

#endif
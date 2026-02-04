#ifndef MASTERTOOLS_ZERO_INIT_H_
#define MASTERTOOLS_ZERO_INIT_H_

#include <string.h>

#define MT_ZERO_INIT(ptr) memset(ptr, 0, sizeof *(ptr));

#endif
#ifndef MASTERTOOLS_ASSERT_H_
#define MASTERTOOLS_ASSERT_H_

#include <stdlib.h>

#include "logging.h"
#include "types.h"

#define MT_ASSERT(expr, ...)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            LFATAL("Assertion '" #expr "' failed: " __VA_ARGS__);                                                      \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

#define MT_UNIMPLEMENTED MT_ASSERT(false, "Unimplemented")

#define MT_UNREACHABLE MT_ASSERT(false, "Unreachable")

#endif
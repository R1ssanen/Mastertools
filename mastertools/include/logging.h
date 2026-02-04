#ifndef MASTERTOOLS_LOGGING_H_
#define MASTERTOOLS_LOGGING_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef MT_ANSI_COLORS
#define MT_TEXT_WHITE(text) "\x1b[37m" text "\x1b[0m"
#define MT_TEXT_CYAN(text) "\x1b[36m" text "\x1b[0m"
#define MT_TEXT_YELLOW_BOLD(text) "\x1b[30;43m" text "\x1b[0m"
#define MT_TEXT_RED_BOLD(text) "\x1b[30;41m" text "\x1b[0m"
#define MT_TEXT_PURPLE_BOLD(text) "\x1b[30;45m" text "\x1b[0m"
#else
#define MT_TEXT_WHITE(text) text
#define MT_TEXT_CYAN(text) text
#define MT_TEXT_YELLOW_BOLD(text) text
#define MT_TEXT_RED_BOLD(text) text
#endif

#define LINFO(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, MT_TEXT_WHITE("[ INFO ]") "  " __VA_ARGS__);                                                   \
        fputc('\n', stdout);                                                                                           \
    } while (0)

#define LWARN(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, MT_TEXT_YELLOW_BOLD("[ WARN ]") "  " __VA_ARGS__);                                             \
        fputc('\n', stderr);                                                                                           \
    } while (0)

#define LERROR(...)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, MT_TEXT_RED_BOLD("[ ERROR ]") " " __VA_ARGS__);                                                \
        fprintf(stderr, " (%s:%d)\n", __func__, __LINE__);                                                             \
    } while (0)

#define LFATAL(...)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, MT_TEXT_PURPLE_BOLD("[ FATAL ]") " " __VA_ARGS__);                                             \
        fprintf(stderr, " (%s:%d)\n", __func__, __LINE__);                                                             \
    } while (0)

#ifndef NDEBUG
#define LDEBUG(...)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, MT_TEXT_CYAN("[ DEBUG ]") " " __VA_ARGS__);                                                    \
        fprintf(stderr, " (%s:%d)\n", __func__, __LINE__);                                                             \
    } while (0)

#define LTRACE(...)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, MT_TEXT_WHITE("[ TRACE ]") " " __VA_ARGS__);                                                   \
        fputc('\n', stderr);                                                                                           \
    } while (0)
#else
#define LDEBUG(...) (void)0
#define LTRACE(...) (void)0
#endif

#endif
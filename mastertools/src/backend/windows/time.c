#if defined(_WIN32) || defined(__CYGWIN__)

#include "system/time.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "logging.h"
#include "mtassert.h"

struct mt_timer
{
    LARGE_INTEGER freq;
    LARGE_INTEGER last;
    LARGE_INTEGER start;
};

mt_timer *mt_timer_create(void)
{
    mt_timer *timer = malloc(sizeof *timer);
    MT_ASSERT(timer != NULL, "Could not allocate memory for timer");

    if (QueryPerformanceFrequency(&timer->freq) == 0)
    {
        LERROR("System does not support high-resolution performance counter");
        goto fail;
    }

    if (QueryPerformanceCounter(&timer->start) == 0)
    {
        LERROR("System does not support high-resolution performance counter");
        goto fail;
    }

    return timer;

fail:
    mt_timer_free(timer);
    return NULL;
}

void mt_timer_free(mt_timer *timer)
{
    if (timer)
    {
        free(timer);
    }
}

double mt_timer_get_elapsed_since_created(const mt_timer *timer)
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    double elapsed_s = (double)(now.QuadPart - timer->start.QuadPart) / (double)timer->freq.QuadPart;
    return elapsed_s * MT_MILLIS_PER_SEC;
}

double mt_timer_get_elapsed(const mt_timer *timer)
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    double elapsed_s = (double)(now.QuadPart - timer->last.QuadPart) / (double)timer->freq.QuadPart;
    return elapsed_s * MT_MILLIS_PER_SEC;
}

void mt_timer_reset(mt_timer *timer)
{
    QueryPerformanceCounter(&timer->last);
}

void mt_sleep(size_t millis)
{
    Sleep(millis);
}

#endif
#ifndef MASTERTOOLS_TIME_H_
#define MASTERTOOLS_TIME_H_

#include "types.h"

#define MT_MILLIS_PER_SEC 1000.0
#define MT_MICROS_PER_SEC 1000000.0
#define MT_NANOS_PER_SEC 1000000000.0

typedef struct mt_timer mt_timer;

mt_timer *mt_timer_create(void);

void mt_timer_free(mt_timer *timer);

double mt_timer_get_elapsed_since_created(const mt_timer *timer);

// returns milliseconds
double mt_timer_get_elapsed(const mt_timer *timer);

void mt_timer_reset(mt_timer *timer);

void mt_sleep(size_t millis);

#endif
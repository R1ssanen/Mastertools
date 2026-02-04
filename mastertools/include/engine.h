#ifndef MASTERTOOLS_ENGINE_H_
#define MASTERTOOLS_ENGINE_H_

#include "map/map.h"
#include "types.h"
#include "utility/mtstring.h"

struct mt_window;
struct mt_timer;

typedef struct mt_engine mt_engine;
struct mt_engine
{
    mt_map map;
    struct mt_window *window;
    struct mt_timer *timer;
    size_t frames_elapsed;
    double delta_time;
    double time_elapsed;
    bool running;
};

bool mt_engine_create(mt_engine *engine);

void mt_engine_free(mt_engine *engine);

bool mt_engine_run(mt_engine *engine);

#endif
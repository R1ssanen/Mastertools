#ifndef MASTERTOOLS_ENGINE_H_
#define MASTERTOOLS_ENGINE_H_

#include "allocator.h"
#include "scene/scene.h"
#include "types.h"
#include "utility/mstring.h"

struct mt_window;

typedef struct mt_engine mt_engine;
struct mt_engine
{
    mt_scene scene;
    mt_allocator alloc;
    struct mt_window *window;
    double delta_time;
    bool running;
};

bool mt_engine_create(mstring level_path, mt_engine *engine);

void mt_engine_free(mt_engine *engine);

bool mt_engine_run(mt_engine *engine);

#endif
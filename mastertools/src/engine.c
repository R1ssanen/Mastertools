#include "engine.h"

#include "allocator.h"
#include "scene/scene.h"
#include "types.h"
#include "window.h"

bool mt_engine_create(mstring level_path, mt_engine *engine)
{
    engine->alloc = create_allocator();
    engine->running = true;
    engine->delta_time = 0.0;

    engine->window = mt_window_create(&engine->alloc, mt_mstring_copy_raw("bingus"), 1440, 900, MT_WINDOW_RESIZABLE);
    if (!engine->window)
    {
        return false;
    }

    return mt_scene_load(&engine->alloc, level_path, &engine->scene);
}

void mt_engine_free(mt_engine *engine)
{
    mt_scene_free(&engine->scene);
    mt_window_free(engine->window);
    free_allocator(&engine->alloc);
}

#include <SDL3/SDL.h>

bool mt_engine_run(mt_engine *engine)
{
    const int fps = 60;

    while (engine->running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                engine->running = false;
                break;
            }
        }

        engine->delta_time += 1.0 / fps;
    }

    return true;
}
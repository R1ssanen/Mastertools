#include "engine.h"

#include <math.h>

#include "rohan.h"
#include "scene/scene.h"
#include "system/text.h"
#include "types.h"
#include "window.h"

#define mt_set_uniform(shader, index, value) (shader)->set_uniform((shader)->instance, index, value, sizeof *value)

bool mt_engine_create(mt_string_view level_path, struct mt_engine *engine)
{
    engine->running = true;
    engine->frames_elapsed = 0;
    engine->delta_time = 0.0;

    engine->window = mt_window_create("bingus", 1440, 900, MT_WINDOW_RESIZABLE);
    if (!engine->window)
    {
        return false;
    }

    return mt_scene_load(level_path, &engine->scene);
}

void mt_engine_free(mt_engine *engine)
{
    mt_scene_free(&engine->scene);
    mt_window_free(engine->window);

#ifdef MT_SANITIZE_FREE
    memset(engine, 0, sizeof *engine);
#endif
}

#include "logging.h"
#include "scene/entity.h"
#include "scene/node.h"
#include <SDL3/SDL.h>

bool mt_engine_run(struct mt_engine *engine)
{
    const int fps = 144;

    int *pixels = _mm_malloc(1440 * 900 * sizeof *pixels, 32);
    float *depth = _mm_malloc(1440 * 900 * sizeof *depth, 32);

    mt_node node = mt_array_get(&engine->scene.nodes, mt_node, 0);
    mt_entity *entity = node.data;
    // entity->shader.set_uniform(entity->shader.instance, 0, &pixels, sizeof(pixels));
    // entity->shader.set_uniform(entity->shader.instance, 1, &depth, sizeof(depth));

    mt_set_uniform(&entity->shader, 0, &pixels);
    mt_set_uniform(&entity->shader, 1, &depth);

    char fps_buf[32];

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

        mt_mesh mesh = mt_array_get(&entity->meshes, mt_mesh, 0);
        float *vertices = malloc(mesh.vertices.size * sizeof(float));
        memcpy(vertices, mesh.vertices.data, mesh.vertices.size * sizeof(float));

        const float size = 700.f;
        for (size_t i = 0; i < mesh.vertices.size; i += 3)
        {
            float x = vertices[i];
            float y = vertices[i + 1];
            float z = vertices[i + 2];

            z += 6.f;
            x /= z;
            y /= z;

            x = (x * size) + (1440.f / 2);
            y = 900 - ((y * size) + (900.f / 2));

            vertices[i] = x;
            vertices[i + 1] = y;
            vertices[i + 2] = z;
        }

        rohan_render(&entity->shader, 1440, vertices, mesh.indices.data, mesh.indices.size, ROHAN_TRIANGLE);

        sprintf(fps_buf, "frame: %zu", engine->frames_elapsed);
        mt_render_text_2d_default(pixels, 1440, mt_string_refer_raw(fps_buf), 10, 10, 16, 16, 0xffffffff);
        mt_render_text_2d_default(pixels, 1440, mt_string_refer_raw("this is also text"), 10, 30, 16, 16, 0xfff0f00f);

        mt_window_render(engine->window, pixels, 1440);
        memset(pixels, 0, 1440 * 900 * sizeof *pixels);

        for (size_t i = 0; i < 1440 * 900; ++i)
        {
            depth[i] = INFINITY;
        }

        engine->frames_elapsed += 1;
        engine->delta_time += 1.0 / fps;
    }

    return true;
}

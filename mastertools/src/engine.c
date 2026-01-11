#include "engine.h"

#include "allocator.h"
#include "scene/scene.h"
#include "types.h"
#include "window.h"

//
#include "rohan.h"

bool mt_engine_create(mt_string_view level_path, mt_engine *engine)
{
    rohan_initialize();

    engine->alloc = create_allocator();
    engine->running = true;
    engine->delta_time = 0.0;

    engine->window = mt_window_create(&engine->alloc, "bingus", 1440, 900, MT_WINDOW_RESIZABLE);
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

#include "logging.h"
#include "scene/entity.h"
#include "scene/node.h"
#include <SDL3/SDL.h>

bool mt_engine_run(mt_engine *engine)
{
    const int fps = 60;

    int *pixels = _mm_malloc(1440 * 900 * sizeof(int), 32);

    mt_entity *entity = engine->scene.root->children[0]->data;

    entity->shader.set_uniform(entity->shader.instance, 0, &pixels, sizeof(pixels));

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

        // mt_mesh mesh = entity->meshes[0];
        // float *vertices = malloc(mesh.vertex_count * sizeof(float));
        // memcpy(vertices, mesh.vertices, mesh.vertex_count * sizeof(float));

        // const float size = 20.f;
        // for (size_t i = 0; i < mesh.vertex_count; i += 3)
        // {
        //     vertices[i + 2] += 1.f;
        //     vertices[i] /= vertices[i + 2];
        //     vertices[i + 1] /= vertices[i + 2];

        //     vertices[i] = (vertices[i] * size) + (1440.f / 2);
        //     vertices[i + 1] = (vertices[i + 1] * size) + (900.f / 2);

        //     // LINFO("%f %f %f\n", vertices[i], vertices[i + 1], vertices[i + 2]);
        // }

        // rohan_render(&entity->shader, 1440, vertices, (const int *)mesh.indices, mesh.index_count, ROHAN_TRIANGLE);

        float z0 = 0.f, z1 = 1.f, z2 = 0.f, z3 = 1.f;
        rohan_render_triangle_raw(&entity->shader, 1440, 0.f, 0.f, 1440.f, 0.f, 1440.f, 900.f, &z0, &z1, &z2);
        rohan_render_triangle_raw(&entity->shader, 1440, 0.f, 0.f, 1440.f, 900.f, 0.f, 900.f, &z0, &z2, &z3);

        mt_window_render(engine->window, pixels, 1440);

        engine->delta_time += 1.0 / fps;
    }

    return true;
}
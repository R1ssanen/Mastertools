#include "engine.h"

#include <SDL3/SDL.h>
#include <math.h>

#include "rohan.h"
#include "scene/scene.h"
#include "system/text.h"
#include "types.h"
#include "window.h"

//
#include "math/3d_utils.h"
#include "math/matrix.h"
#include "math/quat.h"
#include "math/vector.h"

#define mt_set_uniform(spec, instance, index, value_ptr)                                                               \
    memcpy(((char *)instance) + (spec)->uniform_offsets[index], value_ptr, sizeof *(value_ptr))

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

mvec3f rotate_camera(mvec3f forward, float dxm, float dym)
{
    float sensitivity = -0.001f;
    mquat rotation = mt_quat_axis_angle(mt_vec3(0, 1, 0), dxm * sensitivity);
    rotation = mt_quat_mul(mt_quat_axis_angle(mt_vec3(1, 0, 0), dym * sensitivity), rotation);

    forward = mt_quat_rotate_vec3(rotation, forward);
    return mt_vec3_normalize(forward);
}

bool mt_engine_run(struct mt_engine *engine)
{
    const int fps = 144;

    int w = 1440, h = 900;
    float aspect = (float)w / (float)h;

    int *pixels = _mm_malloc(w * h * sizeof *pixels, 32);
    float *depth = _mm_malloc(w * h * sizeof *depth, 32);

    mt_node node = mt_array_get(&engine->scene.nodes, mt_node, 0);
    mt_entity *entity = node.data;

    mt_set_uniform(entity->spec, entity->shader_instance, 0, &pixels);
    mt_set_uniform(entity->spec, entity->shader_instance, 1, &depth);

    float fov = 90.f;
    float far = 20.f;
    float near = 0.1f;

    mvec3f camera_pos = mt_vec3(0.f, 0.f, 0.f);
    mvec3f camera_forward = mt_vec3(0.f, 0.f, -1.f);

    mt_mesh mesh = mt_array_get(&entity->meshes, mt_mesh, 0);
    float *vertices = malloc(mesh.vertices.size * sizeof(float));

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

            else if (e.type == SDL_EVENT_KEY_DOWN)
            {
                switch (e.key.scancode)
                {
                case SDL_SCANCODE_Q:
                    engine->running = false;
                    break;
                case SDL_SCANCODE_W:
                    camera_pos[2] -= 0.1f;
                    break;
                case SDL_SCANCODE_S:
                    camera_pos[2] += 0.1f;
                    break;
                case SDL_SCANCODE_A:
                    camera_pos[0] -= 0.1f;
                    break;
                case SDL_SCANCODE_D:
                    camera_pos[0] += 0.1f;
                    break;
                case SDL_SCANCODE_SPACE:
                    camera_pos[1] += 0.1f;
                    break;
                case SDL_SCANCODE_LSHIFT:
                    camera_pos[1] -= 0.1f;
                    break;

                default:
                    break;
                }
            }

            else if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
                camera_forward = rotate_camera(camera_forward, e.motion.xrel, e.motion.yrel);
            }
        }

        mquat rot = mt_quat_axis_angle(mt_vec3(0.f, 1.f, 0.f), engine->delta_time);
        rot = mt_quat_normalize(rot);

        mmat4f mvp =
            mt_mat4_mul(mt_project(fov, aspect, near, far),
                        mt_mat4_mul(mt_view(camera_pos, camera_forward), mt_transform(mt_vec3(0.f, -1.2f, -7.f))));

        memcpy(vertices, mesh.vertices.data, mesh.vertices.size * sizeof(float));

        for (size_t i = 0; i < mesh.vertices.size; i += 3)
        {
            mvec4f vec = mt_vec3_to_vec4(_mm_loadu_ps(vertices + i), 1.f);

            vec = mt_quat_rotate_vec3(rot, vec);
            vec = mt_mat4_mul_vec4(mvp, mt_vec3_to_vec4(vec, 1.f));

            vec[0] /= vec[3];
            vec[1] /= vec[3];
            vec[2] /= vec[3];

            vec[0] = (vec[0] + 1.f) * (w / 2.f);
            vec[1] = (1.f - vec[1]) * (h / 2.f);

            mt_vec3_store(vertices + i, vec);
        }

        rohan_render(entity->spec, entity->shader_instance, w * sizeof(int), sizeof(int), vertices, mesh.indices.data,
                     mesh.indices.size, ROHAN_TRIANGLE);

        char fps_buf[32];
        sprintf(fps_buf, "frame: %zu", engine->frames_elapsed);
        mt_render_text_2d_default(pixels, w, mt_string_refer_raw(fps_buf), 10, 10, 12, 12, 0xffffffff);

        mt_window_render(engine->window, pixels, w);
        memset(pixels, 0, w * h * sizeof *pixels);
        for (size_t i = 0; i < (size_t)(w * h); ++i)
        {
            depth[i] = INFINITY;
        }

        engine->frames_elapsed += 1;
        engine->delta_time += 1.0 / fps;
    }

    return true;
}

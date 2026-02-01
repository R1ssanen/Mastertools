#include "engine.h"

#include <SDL3/SDL_events.h>
#include <math.h>

#include "rohan.h"
#include "scene/scene.h"
#include "system/text.h"
#include "system/time.h"
#include "types.h"
#include "window.h"

//
#include "math/common.h"
#include "math/matrix.h"
#include "math/quat.h"
#include "math/vector.h"
#include "scene/camera.h"

#define mt_set_uniform(desc, instance, index, value_ptr)                                                               \
    memcpy(((char *)instance) + (desc)->uniform_offsets[index], value_ptr, sizeof *(value_ptr))

static const int fps = 140;
static const int w = 1280;
static const int h = 720;

bool mt_engine_create(mt_string_view level_path, struct mt_engine *engine)
{
    engine->running = true;
    engine->frames_elapsed = 0;

    engine->timer = mt_timer_create();
    if (!engine->timer)
    {
        return false;
    }

    engine->window = mt_window_create("bingus", w, h, MT_WINDOW_RESIZABLE);
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
    mt_timer_free(engine->timer);

#ifdef MT_SANITIZE_FREE
    memset(engine, 0, sizeof *engine);
#endif
}

#include "logging.h"
#include "math/constants.h"
#include "scene/entity.h"
#include "scene/node.h"

mvec3 rotate_camera(mvec3 angle, float dx, float dy)
{
    const float sensitivity = 0.002f;
    const float pitch_limit = MT_RADIANS(89.f);

    float pitch = angle.x - dy * sensitivity;
    float yaw = angle.y - dx * sensitivity;

    return mt_vec3(MT_CLAMP(pitch, -pitch_limit, pitch_limit), yaw, angle.z);
}

void move_camera(mt_camera *camera, SDL_Scancode key)
{
    const mvec3 speed = mt_vec3_all(0.1f);
    mvec3 forward = mt_camera_get_forward(camera);
    mvec3 right = mt_vec3_cross(forward, mt_vec3(0.f, 1.f, 0.f));

    switch (key)
    {
    case SDL_SCANCODE_D:
        camera->pos = mt_vec3_add(camera->pos, mt_vec3_mul(right, speed));
        return;
    case SDL_SCANCODE_A:
        camera->pos = mt_vec3_sub(camera->pos, mt_vec3_mul(right, speed));
        return;
    case SDL_SCANCODE_SPACE:
        camera->pos = mt_vec3_add(camera->pos, mt_vec3_mul(mt_vec3(0.f, 1.f, 0.f), speed));
        return;
    case SDL_SCANCODE_LSHIFT:
        camera->pos = mt_vec3_sub(camera->pos, mt_vec3_mul(mt_vec3(0.f, 1.f, 0.f), speed));
        return;
    case SDL_SCANCODE_W:
        camera->pos = mt_vec3_add(camera->pos, mt_vec3_mul(forward, speed));
        return;
    case SDL_SCANCODE_S:
        camera->pos = mt_vec3_sub(camera->pos, mt_vec3_mul(forward, speed));
        return;
    default:
        return;
    }
}

#include "analytics.h"

bool mt_engine_run(mt_engine *engine)
{
    mt_frame_analytics analytics = mt_analytics_create(engine->timer);

    float aspect = (float)w / (float)h;

    int *pixels = _mm_malloc(w * h * sizeof *pixels, 32);
    int *depth = _mm_malloc(w * h * sizeof *depth, 32);

    // temporary work buffer, move to somewhere else
    float *vertices = malloc(sizeof(float) * 100000);

    for (; engine->running; engine->frames_elapsed += 1)
    {
        mt_timer_reset(engine->timer);
        mt_camera *active_camera = engine->scene.active_camera;

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
                if (e.key.scancode == SDL_SCANCODE_Q)
                {
                    engine->running = false;
                }
                else
                {
                    if (active_camera)
                    {
                        move_camera(active_camera, e.key.scancode);
                    }
                }
            }

            else if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
                if (active_camera)
                {
                    active_camera->angle = rotate_camera(active_camera->angle, e.motion.xrel, e.motion.yrel);
                }
            }
        }

        if (active_camera)
        {
            mvec3 camera_forward = mt_camera_get_forward(active_camera);
            mmat4 view_projection = mt_camera_get_view_projection(active_camera, aspect);

            mt_node_propagate_transform(&engine->scene.root, &engine->scene, mt_mat4_identity());

            mt_array_foreach(&engine->scene.nodes, mt_node, node)
            {
                if (node->kind != MT_NODE_ENTITY)
                {
                    continue;
                }

                mt_entity *entity = node->data;
                mt_set_uniform(entity->desc, entity->shader_instance, 0, &pixels);
                mt_set_uniform(entity->desc, entity->shader_instance, 1, &depth);

                mt_mesh *mesh = mt_array_ptr(&entity->meshes, mt_mesh, 0);
                mmat4 mvp = mt_mat4_mul(view_projection, node->transform);

                for (size_t i = 0; i < mesh->vertices.size; i += 3)
                {
                    mvec3 xyz = mt_vec3_load((float *)mesh->vertices.data + i);
                    mvec4 xyzw = mt_mat4_mul_vec4(mvp, mt_vec3_to_vec4(xyz, 1.f));

                    if ((xyzw.z < -xyzw.w) || (xyzw.z > xyzw.w) || (xyzw.x < -xyzw.w) || (xyzw.x > xyzw.w) ||
                        (xyzw.y < -xyzw.w) || (xyzw.y > xyzw.w))
                    {
                        vertices[i] = NAN;
                        continue;
                    }

                    xyz = mt_vec4_to_vec3(xyzw);
                    xyz = mt_vec3_div(xyz, mt_vec3_all(xyzw.w));

                    xyz.x = fmaf(xyz.x, 0.5f, 0.5f) * (w - 0.5f);
                    xyz.y = fmaf(xyz.y, -0.5f, 0.5f) * (h - 0.5f);
                    xyz.z = fmaf(xyz.z, 0.5f, 0.5f);
                    xyz.z = MT_CLAMP(xyz.z, 0.f, 1.f) * (float)0x00ffffff;

                    mt_vec3_store(xyz, vertices + i);
                }

                size_t attribs = entity->desc->attribute_count + 2;

                for (size_t i = 0; i < mesh->indices.size; i += 3)
                {
                    uint32_t *indices = mesh->indices.data;
                    float *v0 = vertices + indices[i] * attribs;
                    float *v1 = vertices + indices[i + 1] * attribs;
                    float *v2 = vertices + indices[i + 2] * attribs;

                    if (isnan(v0[0]) || isnan(v1[0]) || isnan(v2[0]))
                    {
                        continue;
                    }

                    // (v1[1] - v2[1]) * (v0[0] - v2[0]) + (v2[0] - v1[0]) * (v0[1] - v2[1])
                    if (fmaf(v1[1] - v2[1], v0[0] - v2[0], (v2[0] - v1[0]) * (v0[1] - v2[1])) >= 0.f)
                    {
                        continue;
                    }

                    mt_set_uniform(entity->desc, entity->shader_instance, 2, &i);
                    rohan_rasterize(entity->desc, entity->shader_instance, w * sizeof(int), sizeof(int), v0[0], v0[1],
                                    v1[0], v1[1], v2[0], v2[1], v0 + 2, v1 + 2, v2 + 2);
                }
            }
        }

        {
            int written_total = 0;
            int written_now = 0;

            char text_buf[64];
            written_now = sprintf(text_buf, "ms.a: %.3f ", mt_analytics_get_average_frametime(&analytics));
            mt_render_text_2d_default(pixels, w, mt_string_refer_raw(text_buf), 10, 10, 10, 10, 0xffffffff);

            written_total += written_now + 1;
            written_now = sprintf(text_buf, "ms.r: %.3f ", engine->delta_time);
            mt_render_text_2d_default(pixels, w, mt_string_refer_raw(text_buf), written_total * 10, 10, 10, 10,
                                      0xffffffff);

            written_total += written_now;
            written_now = sprintf(text_buf, "fps: %zu ", (size_t)(MT_MILLIS_PER_SEC / engine->delta_time));
            mt_render_text_2d_default(pixels, w, mt_string_refer_raw(text_buf), written_total * 10, 10, 10, 10,
                                      0xff00ccff);

            written_total += written_now;
            written_now = sprintf(text_buf, "frame: %zu", engine->frames_elapsed);
            mt_render_text_2d_default(pixels, w, mt_string_refer_raw(text_buf), written_total * 10, 10, 10, 10,
                                      0xff00ff00);

            mt_analytics_render_frame_graph(&analytics, pixels, w, 10, 30, 200, 50);
        }

        mt_window_render(engine->window, pixels, w);

        memset(pixels, 0, w * h * sizeof *pixels);
        memset(depth, 0, w * h * sizeof *depth);

        double elapsed = mt_timer_get_elapsed(engine->timer);
        double frame_surplus = MT_MILLIS_PER_SEC / (double)fps - elapsed;
        if (frame_surplus > 0.0)
        {
            mt_sleep(frame_surplus);
        }

        engine->delta_time = mt_timer_get_elapsed(engine->timer);
    }

    free(vertices);
    _mm_free(pixels);
    _mm_free(depth);

    return true;
}

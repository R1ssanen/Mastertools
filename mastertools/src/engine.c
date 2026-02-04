#include "engine.h"

#include <SDL3/SDL_events.h>
#include <math.h>

#include "map/map.h"
#include "rohan.h"
#include "system/text.h"
#include "system/time.h"
#include "types.h"
#include "utility/zero_init.h"
#include "window.h"

//
#include "map/camera.h"
#include "math/common.h"
#include "math/matrix.h"
#include "math/quat.h"
#include "math/vector.h"

#define mt_set_uniform(desc, instance, index, value_ptr)                                                               \
    memcpy(((char *)instance) + (desc)->uniform_offsets[index], value_ptr, sizeof *(value_ptr))

static const int fps = 150;
static const int w = 1280;
static const int h = 720;

bool mt_engine_create(mt_engine *engine)
{
    rohan_init();
    MT_ZERO_INIT(engine);

    engine->timer = mt_timer_create();
    if (!engine->timer)
    {
        goto fail;
    }

    engine->window = mt_window_create("bingus", w, h, MT_WINDOW_RESIZABLE);
    if (!engine->window)
    {
        goto fail;
    }

    engine->running = true;
    return true;

fail:
    mt_engine_free(engine);
    return false;
}

void mt_engine_free(mt_engine *engine)
{
    if (!engine)
    {
        return;
    }

    mt_map_free(&engine->map);
    mt_window_free(engine->window);
    mt_timer_free(engine->timer);

    MT_ZERO_INIT(engine);
}

#include "logging.h"
#include "map/entity.h"
#include "map/node.h"
#include "math/constants.h"

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

    mt_texture *pixels = mt_texture_create(w, h, MT_TEXTURE_RGBA8888);
    mt_texture *depth = mt_texture_create(w, h, MT_TEXTURE_RGBA8888);

    // temporary work buffer, move to somewhere else
    float *vertices = malloc(sizeof(float) * 3000000);

    for (; engine->running; engine->frames_elapsed += 1)
    {
        mt_timer_reset(engine->timer);

        mt_camera *active_camera = mt_array_ptr(&engine->map.cameras, mt_camera, engine->map.active_camera_id);

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
                    break;
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
            mmat4 projection =
                mt_project_reverse(active_camera->r_fov, aspect, active_camera->near, active_camera->far);
            mmat4 view_projection =
                mt_mat4_mul(projection, mt_view(active_camera->pos, mt_camera_get_forward(active_camera)));

            mt_node_propagate_transform(mt_array_ptr(&engine->map.nodes, mt_node, 0), &engine->map, mt_mat4_identity(),
                                        mt_timer_get_elapsed_since_created(engine->timer));

            mt_array_foreach(&engine->map.nodes, mt_node, node)
            {
                if (node->kind != MT_NODE_ENTITY)
                {
                    continue;
                }

                mt_entity *entity = node->data;
                mt_set_uniform(entity->desc, entity->shader_instance, 0, &pixels);
                mt_set_uniform(entity->desc, entity->shader_instance, 1, &depth);
                mt_set_uniform(entity->desc, entity->shader_instance, 4, &entity->texture);

                __m256i layer_mask = _mm256_set1_epi32(entity->depth_layer << 29);
                mt_set_uniform(entity->desc, entity->shader_instance, 3, &layer_mask);

                mt_mesh *mesh = mt_array_ptr(&entity->meshes, mt_mesh, 0);
                mmat4 mvp = mt_mat4_mul(node->view_parented ? projection : view_projection, node->transform);

                for (size_t i = 0, j = 0; i < mesh->vertices.size; i += 3, j += 4)
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

                    xyz.x = fmaf(xyz.x, 0.5f, 0.5f) * (w - 1.f);
                    xyz.y = fmaf(xyz.y, -0.5f, 0.5f) * (h - 1.f);
                    xyz.z = fmaf(xyz.z, 0.5f, 0.5f);
                    xyz.z = MT_CLAMP(xyz.z, 0.f, 1.f) * (float)0x03ffffff;

                    mt_vec3_store(xyz, vertices + i);
                    // printf("%f %f %f\n", xyz.x, xyz.y, xyz.z);
                    // mt_vec4_store(mt_vec3_to_vec4(xyz, 1.f), vertices + j);
                }

                uint *indices = mesh->indices.data;
                for (size_t i = 0; i < mesh->indices.size; i += 7)
                {
                    uint id = indices[i];
                    mt_set_uniform(entity->desc, entity->shader_instance, 2, &id);

                    float *v0 = vertices + indices[i + 1] * 3;
                    float *v1 = vertices + indices[i + 3] * 3;
                    float *v2 = vertices + indices[i + 5] * 3;

                    if (isnan(v0[0]) || isnan(v1[0]) || isnan(v2[0]))
                    {
                        continue;
                    }
                    else if (fmaf(v1[1] - v2[1], v0[0] - v2[0], (v2[0] - v1[0]) * (v0[1] - v2[1])) >= 0.f)
                    {
                        continue;
                    }

                    float *uvs = mesh->uvs.data;
                    float *uv0 = uvs + indices[i + 2] * 2;
                    float *uv1 = uvs + indices[i + 4] * 2;
                    float *uv2 = uvs + indices[i + 6] * 2;

                    float vertex0[] = {v0[0], v0[1], v0[2], uv0[0], uv0[1]};
                    float vertex1[] = {v1[0], v1[1], v1[2], uv1[0], uv1[1]};
                    float vertex2[] = {v2[0], v2[1], v2[2], uv2[0], uv2[1]};

                    rohan_rasterize(entity->desc, entity->shader_instance, w * sizeof(int), sizeof(int), vertex0,
                                    vertex1, vertex2);

                    // render_line(pixels, w, v0[0], v0[1], v1[0], v1[1], 0xffcccccc);
                    // render_line(pixels, w, v1[0], v1[1], v2[0], v2[1], 0xffcccccc);
                    // render_line(pixels, w, v2[0], v2[1], v0[0], v0[1], 0xffcccccc);
                }
            }
        }

        if (1)
        {
            int written_total = 0;
            int written_now = 0;

            char text_buf[64];
            written_now = sprintf(text_buf, "ms.a: %.3f ", mt_analytics_get_average_frametime(&analytics));
            mt_render_text_2d_default(pixels, mt_string_refer_raw(text_buf), 10, 10, 10, 10, 0xff0000ff);

            written_total += written_now + 1;
            written_now = sprintf(text_buf, "ms.r: %.3f ", engine->delta_time);
            mt_render_text_2d_default(pixels, mt_string_refer_raw(text_buf), written_total * 10, 10, 10, 10,
                                      0xff0000ff);

            written_total += written_now;
            written_now = sprintf(text_buf, "fps: %zu ", (size_t)(MT_MILLIS_PER_SEC / engine->delta_time));
            mt_render_text_2d_default(pixels, mt_string_refer_raw(text_buf), written_total * 10, 10, 10, 10,
                                      0xff00ccff);

            written_total += written_now;
            written_now = sprintf(text_buf, "frame: %zu", engine->frames_elapsed);
            mt_render_text_2d_default(pixels, mt_string_refer_raw(text_buf), written_total * 10, 10, 10, 10,
                                      0xff00ff00);

            written_total = 0;
            written_now = sprintf(text_buf, "t: %dx%d@%dHz ", w, h, fps);
            mt_render_text_2d_default(pixels, mt_string_refer_raw(text_buf), 10, 25, 10, 10, 0xffcccccc);

            written_total += written_now + 1;
            written_now = sprintf(text_buf, "w: %dx%d", 1440, 900);
            mt_render_text_2d_default(pixels, mt_string_refer_raw(text_buf), 10 * written_total, 25, 10, 10,
                                      0xffcccccc);

            mt_analytics_render_frame_graph(&analytics, pixels, 10, 50, 200, 50);
        }

        mt_window_render(engine->window, pixels);

        mt_texture_clear(pixels, 0);
        mt_texture_clear(depth, 0);

        double elapsed = mt_timer_get_elapsed(engine->timer);
        double frame_surplus = MT_MILLIS_PER_SEC / (double)fps - elapsed;
        if (frame_surplus > 0.0)
        {
            mt_sleep(frame_surplus);
        }

        engine->delta_time = mt_timer_get_elapsed(engine->timer);
        engine->time_elapsed += engine->delta_time;
    }

    free(vertices);
    mt_texture_free(pixels);
    mt_texture_free(depth);

    return true;
}

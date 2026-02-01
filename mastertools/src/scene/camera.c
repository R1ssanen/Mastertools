#include "scene/camera.h"

#include "cJSON.h"
#include "math/constants.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "scene/scene.h"

void mt_camera_free(mt_camera *camera)
{
    free(camera);
}

mt_camera *parse_node_camera_json(struct cJSON *object, mt_scene *scene)
{
    mt_camera *camera = malloc(sizeof(mt_camera));

    cJSON *active = cJSON_GetObjectItemCaseSensitive(object, "active");
    if (active && (active->valueint != 0))
    {
        scene->active_camera = camera;
    }

    cJSON *pos = cJSON_GetObjectItemCaseSensitive(object, "pos");
    if (!pos)
    {
        camera->pos = mt_vec3(0.f, 0.f, 0.f);
    }

    cJSON *angle = cJSON_GetObjectItemCaseSensitive(object, "angle");
    if (!angle)
    {
        camera->angle = mt_vec3(0.f, 0.f, 0.f);
    }

    cJSON *fov = cJSON_GetObjectItemCaseSensitive(object, "fov");
    if (!fov)
    {
        camera->r_fov = MT_RADIANS(90.f);
    }

    cJSON *range = cJSON_GetObjectItemCaseSensitive(object, "range");
    if (!range)
    {
        camera->range = mt_vec2(0.1f, 100.f);
    }

    return camera;
}

mvec3 mt_camera_get_forward(mt_camera *camera)
{
    mquat yaw = mt_quat_axis_angle(mt_vec3(0.f, 1.f, 0.f), camera->angle.y);

    mvec3 forward = mt_quat_rotate_vec3(yaw, mt_vec3(0.f, 0.f, -1.f));
    mvec3 local_right = mt_vec3_cross(mt_vec3_normalize(forward), mt_vec3(0.f, 1.f, 0.f));

    mquat pitch = mt_quat_axis_angle(local_right, camera->angle.x);
    return mt_vec3_normalize(mt_quat_rotate_vec3(pitch, forward));
}

mmat4 mt_camera_get_view_projection(mt_camera *camera, float aspect)
{
    return mt_mat4_mul(mt_project_reverse(camera->r_fov, aspect, camera->range.x, camera->range.y),
                       mt_view(camera->pos, mt_camera_get_forward(camera)));
}

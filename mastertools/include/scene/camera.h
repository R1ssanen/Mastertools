#ifndef MASTERTOOLS_CAMERA_H_
#define MASTERTOOLS_CAMERA_H_

#include "math/common.h"
#include "math/vec3.h"
#include "types.h"

struct cJSON;
struct mt_scene;

typedef struct mt_camera mt_camera;
struct mt_camera
{
    mvec3 pos;
    mvec3 angle;
    mvec2 range;
    float r_fov;
    bool active;
};

void mt_camera_free(mt_camera *camera);

mt_camera *parse_node_camera_json(struct cJSON *object, struct mt_scene *scene);

mvec3 mt_camera_get_forward(mt_camera *camera);

mmat4 mt_camera_get_view_projection(mt_camera *camera, float aspect);

#endif
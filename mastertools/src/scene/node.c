#include "scene/node.h"

#include "logging.h"
#include "math/mat4.h"
#include "scene/camera.h"
#include "scene/entity.h"
#include "scene/scene.h"

void mt_node_free(mt_node *node)
{
    switch (node->kind)
    {
    case MT_NODE_ENTITY:
        mt_entity_free(node->data);
        break;
    case MT_NODE_CAMERA:
        mt_camera_free(node->data);

    default:
        break;
    }

    mt_array_free(&node->children);

#ifdef MT_SANITIZE_FREE
    memset(node, 0, sizeof *node);
#endif
}

void mt_node_propagate_transform(mt_node *node, mt_scene *scene, mmat4 prop)
{
    switch (node->kind)
    {
    case MT_NODE_ROOT:
    case MT_NODE_ENTITY:
        node->transform = mt_mat4_identity();
        break;
    case MT_NODE_CAMERA:
        mt_camera *camera = node->data;
        node->transform = mt_mat4_mul(mt_transform(camera->pos),
                                      mt_rotation_euler_angles(camera->angle.x, camera->angle.y, camera->angle.z));
        break;
    default:
        LERROR("Uknown node kind %d", node->kind);
        return;
    }

    node->transform = mt_mat4_mul(prop, node->transform);

    mt_array_foreach(&node->children, uint32_t, node_id)
    {
        mt_node_propagate_transform(mt_array_ptr(&scene->nodes, mt_node, *node_id), scene, node->transform);
    }
}

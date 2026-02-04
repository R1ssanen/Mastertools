#include "map/node.h"

#include "logging.h"
#include "map/camera.h"
#include "map/entity.h"
#include "map/map.h"
#include "math/mat4.h"
#include "utility/zero_init.h"

void mt_node_free(mt_node *node)
{
    if (!node)
    {
        return;
    }

    switch (node->kind)
    {
    case MT_NODE_ENTITY:
        mt_entity_free(node->data);
        break;
    default:
        break;
    }

    mt_array_free(&node->children);

    MT_ZERO_INIT(node);
}

#include "math/constants.h"

void mt_node_propagate_transform(mt_node *node, mt_map *map, mmat4 prop, double elapsed)
{
    switch (node->kind)
    {
    case MT_NODE_ROOT:
    case MT_NODE_CAMERA:
    case MT_NODE_ENTITY:
        if (node->view_parented)
        {
            mvec3 magnitude = mt_vec3(0.003f, 0.005f, 0.002f);
            mvec3 speed = mt_vec3(0.0015f, 0.0015f, 0.0015f);
            mvec3 offset = mt_vec3(magnitude.x * sinf(elapsed * speed.x), // subtle left-right sway
                                   magnitude.y * sinf(elapsed * speed.y), // up-down bob
                                   magnitude.z * cosf(elapsed * speed.z)  // forward-back tiny motion
            );
            node->transform = mt_translate(offset);
        }
        else
        {
            node->transform = mt_mat4_identity();
        }
        break;

    default:
        LERROR("Uknown node kind %d", node->kind);
        return;
    }

    node->transform = mt_mat4_mul(prop, node->transform);
    mt_array_foreach(&node->children, uint, node_id)
    {
        mt_node_propagate_transform(mt_array_ptr(&map->nodes, mt_node, *node_id), map, node->transform, elapsed);
    }
}

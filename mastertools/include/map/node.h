#ifndef MASTERTOOLS_NODE_H_
#define MASTERTOOLS_NODE_H_

#include "math/mat4.h"
#include "types.h"
#include "utility/array.h"

struct mt_map;

enum mt_node_kind
{
    MT_NODE_INVALID,
    MT_NODE_ROOT,
    MT_NODE_CAMERA,
    MT_NODE_ENTITY,
    MT_NODE_TRANSFORM,
};

typedef struct mt_node mt_node;
struct mt_node
{
    mt_array_of(uint) children;
    mmat4 transform;
    void *data;
    uint parent_id;
    uint self_id;
    enum mt_node_kind kind;
    bool view_parented;
};

void mt_node_free(mt_node *node);

void mt_node_propagate_transform(mt_node *node, struct mt_map *map, mmat4 prop, double delta_time);

#endif
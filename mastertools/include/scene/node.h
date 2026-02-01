#ifndef MASTERTOOLS_NODE_H_
#define MASTERTOOLS_NODE_H_

#include "math/mat4.h"
#include "types.h"
#include "utility/array.h"

struct mt_scene;

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
    mt_array_of(uint32_t) children;
    mmat4 transform;
    void *data;
    uint32_t parent;
    enum mt_node_kind kind;
    bool dirty;
};

void mt_node_propagate_transform(mt_node *node, struct mt_scene *scene, mmat4 prop);

void mt_node_free(mt_node *node);

#endif
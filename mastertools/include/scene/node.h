#ifndef MASTERTOOLS_NODE_H_
#define MASTERTOOLS_NODE_H_

#include "utility/array.h"

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
    void *data;
    uint32_t parent;
    enum mt_node_kind kind;
};

void mt_node_free(mt_node *node);

#endif
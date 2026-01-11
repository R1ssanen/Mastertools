#ifndef MASTERTOOLS_NODE_H_
#define MASTERTOOLS_NODE_H_

#include "utility/array.h"

enum
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
    mt_array children; // mt_node*
    const struct mt_node *parent;
    void *data;
    int kind;
};

mt_node *mt_node_create(void);

void mt_node_free(mt_node *node);

// bool mt_node_insert_child(mt_node *parent, mt_node *child);

#endif
#ifndef MASTERTOOLS_NODE_H_
#define MASTERTOOLS_NODE_H_

#if !defined MT_MAX_NODE_CHILDREN
#define MT_MAX_NODE_CHILDREN 16
#endif

enum
{
    MT_NODE_NONE,
    MT_NODE_ROOT,
    MT_NODE_CAMERA,
    MT_NODE_ENTITY,
    MT_NODE_TRANSFORM,
};

typedef struct mt_node mt_node;
struct mt_node
{
    struct mt_node *children[MT_MAX_NODE_CHILDREN];
    const struct mt_node *parent;
    void *data;
    int child_count;
    int kind;
};

bool mt_node_insert_child(mt_node *parent, mt_node *child);

#endif
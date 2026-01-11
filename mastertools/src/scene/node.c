#include "scene/node.h"

#include "logging.h"
#include "scene/entity.h"

mt_node *mt_node_create(void)
{
    mt_node *node = malloc(sizeof(mt_node));
    if (!node)
    {
        LFATAL("Memory allocation for scene node failed");
    }

    node->children = mt_array_create(sizeof(mt_node *));
    return node;
}

void mt_node_free(mt_node *node)
{
    switch (node->kind)
    {
    case MT_NODE_ENTITY:
        mt_entity_free(node->data);
        break;

    default:
        break;
    }

    mt_array_foreach(&node->children, mt_node *, child)
    {
        mt_node_free(*child);
    }

    mt_array_free(&node->children);
    free(node);
}

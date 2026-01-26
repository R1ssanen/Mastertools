#include "scene/node.h"

#include "logging.h"
#include "scene/entity.h"

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

    mt_array_free(&node->children);

#ifdef MT_SANITIZE_FREE
    memset(node, 0, sizeof *node);
#endif
}

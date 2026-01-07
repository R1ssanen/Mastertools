#include "scene/scene.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "cJSON.h"
#include "logging.h"
#include "scene/entity.h"
#include "scene/node.h"
#include "utility/file.h"

static mt_node *parse_scene_json(cJSON *object, mt_allocator *alloc, mt_scene *scene, mt_node *parent)
{
    cJSON *type = cJSON_GetObjectItemCaseSensitive(object, "type");
    if (!type)
    {
        LERROR("Scene nodes must have a type");
        return NULL;
    }

    mt_node *node = allocate(alloc, sizeof(mt_node));
    node->parent = parent;
    node->child_count = 0;

    if (parent)
    {
        parent->children[parent->child_count++] = node;
    }

    if (strcmp(type->valuestring, "root") == 0)
    {
        node->data = NULL;
    }
    else if (strcmp(type->valuestring, "entity") == 0)
    {
        node->data = parse_node_entity_json(alloc, object);
    }
    else
    {
        LERROR("Unknown scene node type '%s'", type->valuestring);
        deallocate(node);
        return NULL;
    }

    cJSON *children = cJSON_GetObjectItemCaseSensitive(object, "children");
    if (children)
    {
        for (cJSON *child = children->child; child != NULL; child = child->next)
        {
            node->children[node->child_count++] = parse_scene_json(child, alloc, scene, node);
        }
    }

    scene->nodes[scene->node_count++] = node;
    return node;
}

bool mt_scene_load(mt_allocator *alloc, mstring path, mt_scene *scene)
{
    byte *buffer;
    size_t count;
    if (!mt_read_file(path, &buffer, &count))
    {
        return false;
    }

    scene->path = mt_mstring_copy(path);
    scene->name = mt_get_file_name(path);
    scene->node_count = 0;

    cJSON *root = cJSON_ParseWithLength((char *)buffer, count);
    free(buffer);

    scene->root = parse_scene_json(root, alloc, scene, NULL);
    cJSON_Delete(root);

    return true;
}

void mt_scene_free(mt_scene *scene)
{
    for (int i = 0; i < scene->node_count; ++i)
    {
        deallocate(scene->nodes[i]);
    }
}

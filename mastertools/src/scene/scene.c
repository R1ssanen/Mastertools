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
#include "utility/hash.h"

static mt_node *parse_scene_json(cJSON *object, mt_allocator *alloc, mt_scene *scene, mt_node *parent)
{
    cJSON *type = cJSON_GetObjectItemCaseSensitive(object, "type");
    if (!type)
    {
        LERROR("Scene node must have a type");
        return NULL;
    }

    mt_node *node = allocate(alloc, sizeof(mt_node));
    node->parent = parent;
    node->child_count = 0;

    if (parent)
    {
        if (parent->child_count >= MT_MAX_NODE_CHILDREN)
        {
            LERROR("MT_MAX_NODE_CHILDREN exceeded");
            exit(1);
        }
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
    else if (strcmp(type->valuestring, "camera") == 0)
    {
        LERROR("Not implemented");
        exit(1);
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
            if (node->child_count >= MT_MAX_NODE_CHILDREN)
            {
                LERROR("MT_MAX_NODE_CHILDREN exceeded");
                exit(1);
            }
            node->children[node->child_count++] = parse_scene_json(child, alloc, scene, node);
        }
    }

    if (scene->node_count >= MT_MAX_SCENE_NODES)
    {
        LERROR("MT_MAX_SCENE_NODES exceeded");
        exit(1);
    }
    scene->nodes[scene->node_count++] = node;
    return node;
}

bool mt_scene_load(mt_allocator *alloc, mt_string_view path, mt_scene *scene)
{
    byte *buffer;
    size_t count;
    if (!mt_file_read(path, &buffer, &count))
    {
        return false;
    }

    scene->path = mt_string_copy_view(path);
    if (!mt_file_split_directory_name(path, NULL, &scene->name))
    {
        LERROR("Resource path provided not valid");
    }

    scene->node_count = 0;

    cJSON *root = cJSON_ParseWithLength((const char *)buffer, count);
    scene->root = parse_scene_json(root, alloc, scene, NULL);

    cJSON_Delete(root);
    free(buffer);

    if (!scene->root)
    {
        LERROR("Could not load scene '%s'", path.str);
        return false;
    }

    return true;
}

void mt_scene_free(mt_scene *scene)
{
    for (int i = 0; i < scene->node_count; ++i)
    {
        deallocate(scene->nodes[i]);
    }

    mt_string_free(&scene->name);
    mt_string_free(&scene->path);
}

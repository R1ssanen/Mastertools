#include "scene/scene.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "logging.h"
#include "scene/entity.h"
#include "scene/node.h"
#include "utility/file.h"
#include "utility/hash.h"

static mt_node *parse_scene_json(cJSON *object, mt_scene *scene, mt_node *parent)
{
    cJSON *type = cJSON_GetObjectItemCaseSensitive(object, "type");
    if (!type)
    {
        LERROR("Scene node must have a type");
        return false;
    }

    mt_node *node = mt_node_create();
    node->parent = parent;

    if (parent)
    {
        mt_array_push(&parent->children, &node);
    }

    if (strcmp(type->valuestring, "root") == 0)
    {
        node->kind = MT_NODE_ROOT;
        node->data = NULL;
    }
    else if (strcmp(type->valuestring, "entity") == 0)
    {
        node->kind = MT_NODE_ENTITY;
        node->data = parse_node_entity_json(object);
    }
    else if (strcmp(type->valuestring, "camera") == 0)
    {
        node->kind = MT_NODE_CAMERA;
        LERROR("Not implemented");
        exit(1);
    }
    else
    {
        LERROR("Unknown scene node type '%s'", type->valuestring);
        return NULL;
    }

    cJSON *children = cJSON_GetObjectItemCaseSensitive(object, "children");
    if (children)
    {
        for (cJSON *child = children->child; child != NULL; child = child->next)
        {
            parse_scene_json(child, scene, node);
        }
    }

    mt_array_push(&scene->nodes, &node);
    return node;
}

bool mt_scene_load(mt_string_view path, mt_scene *scene)
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
        LERROR("Scene path provided not valid");
        return false;
    }

    cJSON *root = cJSON_ParseWithLength((const char *)buffer, count);
    scene->nodes = mt_array_create(sizeof(mt_node *));
    scene->root = parse_scene_json(root, scene, NULL);

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
    mt_node_free(scene->root);
    mt_array_free(&scene->nodes);

    mt_string_free(&scene->name);
    mt_string_free(&scene->path);

#if defined(MT_SANITIZE_FREE)
    memset(scene, 0, sizeof(*scene));
#endif
}

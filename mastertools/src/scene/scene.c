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

static bool parse_scene_json(cJSON *object, mt_scene *scene, mt_node *parent)
{
    cJSON *type = cJSON_GetObjectItemCaseSensitive(object, "type");
    if (!type)
    {
        LERROR("Scene node must have a type");
        return false;
    }

    uint32_t node_index = scene->nodes.size;
    mt_node *node = mt_array_push(&scene->nodes, NULL);
    node->children = mt_array_create(sizeof(uint32_t));

    if (strcmp(type->valuestring, "entity") == 0)
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
        return false;
    }

    cJSON *children = cJSON_GetObjectItemCaseSensitive(object, "children");
    if (!children)
    {
        return true;
    }

    for (cJSON *child = children->child; child != NULL; child = child->next)
    {
        if (!parse_scene_json(child, scene, node))
        {
            return false;
        }
    }

    mt_array_push(&parent->children, &node_index);
    return true;
}

bool mt_scene_load(mt_string_view path, mt_scene *scene)
{
    char *buffer = NULL;
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

    cJSON *json = cJSON_ParseWithLength((const char *)buffer, count);
    scene->nodes = mt_array_create(sizeof(mt_node));
    scene->root.children = mt_array_create(sizeof(uint32_t));
    scene->root.kind = MT_NODE_ROOT;

    for (cJSON *child = json->child; child != NULL; child = child->next)
    {
        if (!parse_scene_json(child, scene, &scene->root))
        {
            cJSON_Delete(json);
            free(buffer);
            mt_scene_free(scene);
            return false;
        }
    }

    cJSON_Delete(json);
    free(buffer);
    return true;
}

void mt_scene_free(mt_scene *scene)
{
    mt_array_foreach(&scene->nodes, mt_node, node)
    {
        mt_node_free(node);
    }
    mt_array_free(&scene->nodes);

    mt_node_free(&scene->root);

    mt_string_free(&scene->name);
    mt_string_free(&scene->path);

#ifdef MT_SANITIZE_FREE
    memset(scene, 0, sizeof *scene);
#endif
}

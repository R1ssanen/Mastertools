#include "map/map.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "logging.h"
#include "map/camera.h"
#include "map/entity.h"
#include "map/node.h"
#include "utility/file.h"
#include "utility/hash.h"
#include "utility/zero_init.h"

static bool parse_scene_json(cJSON *object, mt_map *map, uint parent_id)
{
    cJSON *type = cJSON_GetObjectItemCaseSensitive(object, "type");
    if (!type)
    {
        LERROR("Scene node must have a type");
        return false;
    }

    uint node_id = map->nodes.size;
    mt_node *node = mt_array_push(&map->nodes, NULL);
    node->children = mt_array_create(sizeof(uint));
    node->self_id = node_id;

    mt_string_view kind = mt_string_refer_raw(type->valuestring);
    if (mt_string_equal_vr(kind, "entity"))
    {
        node->kind = MT_NODE_ENTITY;
        node->data = parse_node_entity_json(object);
    }
    else if (mt_string_equal_vr(kind, "camera"))
    {
        node->kind = MT_NODE_CAMERA;
        node->data = parse_node_camera_json(object, map);
    }
    else
    {
        LERROR("Unknown map node type '%s'", type->valuestring);
        return false;
    }

    cJSON *children = cJSON_GetObjectItemCaseSensitive(object, "nodes");
    if (children)
    {
        for (cJSON *child = children->child; child != NULL; child = child->next)
        {
            if (!parse_scene_json(child, map, node_id))
            {
                LERROR("Could not parse map");
                return false;
            }
        }
    }

    mt_node *parent = mt_array_ptr(&map->nodes, mt_node, parent_id);

    node->view_parented = parent->kind == MT_NODE_CAMERA ? true : false;

    mt_array_push(&parent->children, &node_id);
    return true;
}

bool mt_map_load(mt_string_view path, mt_map *map)
{

    MT_ZERO_INIT(map);
    cJSON *json = NULL;

    char *buffer = NULL;
    size_t count;
    if (!mt_file_read(path, &buffer, &count))
    {
        LERROR("Could not read map file ");
        goto fail;
    }

    map->path = mt_string_copy_view(path);
    if (!mt_file_split_directory_name(path, NULL, &map->name))
    {
        LERROR("Scene path provided not valid");
        goto fail;
    }

    json = cJSON_ParseWithLength((const char *)buffer, count);
    map->nodes = mt_array_create(sizeof(mt_node));
    map->cameras = mt_array_create(sizeof(mt_camera));
    map->active_camera_id = -1;

    mt_node *root = mt_array_push(&map->nodes, NULL);
    root->children = mt_array_create(sizeof(uint));
    root->kind = MT_NODE_ROOT;
    root->self_id = 0;
    root->view_parented = false;

    for (cJSON *child = json->child; child != NULL; child = child->next)
    {
        if (!parse_scene_json(child, map, 0))
        {
            goto fail;
        }
    }

    if (map->active_camera_id == -1)
    {
        LWARN("No active camera set for map '%s'", path.str);
    }

    cJSON_Delete(json);
    free(buffer);

    return true;

fail:
    if (buffer)
    {
        free(buffer);
    }
    if (json)
    {
        cJSON_Delete(json);
    }

    mt_map_free(map);
    return false;
}

void mt_map_free(mt_map *map)
{
    if (!map)
    {
        return;
    }

    mt_array_foreach(&map->nodes, mt_node, node)
    {
        mt_node_free(node);
    }

    mt_array_free(&map->cameras);
    mt_array_free(&map->nodes);

    mt_string_free(&map->name);
    mt_string_free(&map->path);

    MT_ZERO_INIT(map);
}

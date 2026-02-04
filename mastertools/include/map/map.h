#ifndef MASTERTOOLS_SCENE_H_
#define MASTERTOOLS_SCENE_H_

#include "node.h"
#include "types.h"
#include "utility/array.h"
#include "utility/mtstring.h"

struct mt_camera;

typedef struct mt_map mt_map;
struct mt_map
{
    mt_array_of(mt_node) nodes;
    mt_array_of(mt_camera) cameras;
    mt_string name;
    mt_string path;
    int active_camera_id;
};

bool mt_map_load(mt_string_view path, mt_map *map);

void mt_map_free(mt_map *map);

#endif
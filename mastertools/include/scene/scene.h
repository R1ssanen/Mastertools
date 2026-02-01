#ifndef MASTERTOOLS_SCENE_H_
#define MASTERTOOLS_SCENE_H_

#include "node.h"
#include "types.h"
#include "utility/array.h"
#include "utility/mtstring.h"

struct mt_camera;

typedef struct mt_scene mt_scene;
struct mt_scene
{
    mt_array_of(mt_node) nodes;
    mt_node root;
    mt_string name;
    mt_string path;
    struct mt_camera *active_camera;
};

bool mt_scene_load(mt_string_view path, mt_scene *scene);

void mt_scene_free(mt_scene *scene);

#endif
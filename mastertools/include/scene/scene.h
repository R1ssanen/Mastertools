#ifndef MASTERTOOLS_SCENE_H_
#define MASTERTOOLS_SCENE_H_

#include "types.h"
#include "utility/array.h"
#include "utility/mstring.h"

struct mt_node;

typedef struct mt_scene mt_scene;
struct mt_scene
{
    mt_array nodes; // mt_node*
    mt_string name, path;
    struct mt_node *root;
};

bool mt_scene_load(mt_string_view path, mt_scene *scene);

void mt_scene_free(mt_scene *scene);

#endif
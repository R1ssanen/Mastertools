#ifndef MASTERTOOLS_SCENE_H_
#define MASTERTOOLS_SCENE_H_

#include "types.h"
#include "utility/mstring.h"

#if !defined MT_MAX_SCENE_NODES
#define MT_MAX_SCENE_NODES 32
#endif

struct mt_allocator;
struct mt_node;

typedef struct mt_scene mt_scene;
struct mt_scene
{
    struct mt_node *nodes[MT_MAX_SCENE_NODES];
    mstring name, path;
    struct mt_node *root;
    int node_count;
};

bool mt_scene_load(struct mt_allocator *alloc, mstring path, mt_scene *scene);

void mt_scene_free(mt_scene *scene);

#endif
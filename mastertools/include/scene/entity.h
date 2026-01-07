#ifndef MASTERTOOLS_ENTITY_H_
#define MASTERTOOLS_ENTITY_H_

#if !defined(MT_MAX_ENTITY_MESHES)
#define MT_MAX_ENTITY_MESHES 8
#endif

#include "mesh.h"
#include "types.h"

struct mt_allocator;
struct cJSON;

typedef struct mt_entity mt_entity;
struct mt_entity
{
    mt_mesh mesh[MT_MAX_ENTITY_MESHES];
    size_t mesh_count;
};

void *parse_node_entity_json(struct mt_allocator *alloc, const struct cJSON *object);

#endif
#ifndef MASTERTOOLS_ENTITY_H_
#define MASTERTOOLS_ENTITY_H_

#if !defined(MT_MAX_ENTITY_MESHES)
#define MT_MAX_ENTITY_MESHES 8
#endif

#include "mesh.h"
#include "types.h"

// rohan
#include "shader.h"
#include "system/library.h"

struct mt_allocator;
struct cJSON;

typedef struct mt_entity mt_entity;
struct mt_entity
{
    rohan_shader_object shader;
    mt_mesh meshes[MT_MAX_ENTITY_MESHES];
    mt_library shader_lib;
    size_t mesh_count;
};

void *parse_node_entity_json(struct mt_allocator *alloc, const struct cJSON *object);

#endif
#ifndef MASTERTOOLS_ENTITY_H_
#define MASTERTOOLS_ENTITY_H_

#include "mesh.h"
#include "types.h"
#include "utility/array.h"

// rohan
#include "shader.h"
#include "system/library.h"

struct cJSON;

typedef struct mt_entity mt_entity;
struct mt_entity
{
    rohan_shader_object shader;
    mt_array meshes; // mt_mesh
    mt_library shader_lib;
};

void mt_entity_free(mt_entity *entity);

mt_entity *parse_node_entity_json(const struct cJSON *object);

#endif
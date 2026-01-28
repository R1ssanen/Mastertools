#ifndef MASTERTOOLS_ENTITY_H_
#define MASTERTOOLS_ENTITY_H_

#include "mesh.h"
#include "rohan.h"
#include "system/library.h"
#include "types.h"
#include "utility/array.h"

struct cJSON;

typedef struct mt_entity mt_entity;
struct mt_entity
{
    rohan_shader_spec *spec;
    mt_array_of(mt_mesh) meshes;
    mt_library shader_lib;
    void *shader_instance;
};

void mt_entity_free(mt_entity *entity);

mt_entity *parse_node_entity_json(const struct cJSON *object);

#endif
#ifndef MASTERTOOLS_MESH_H_
#define MASTERTOOLS_MESH_H_

#include "types.h"
#include "utility/array.h"

typedef struct mt_mesh mt_mesh;
struct mt_mesh
{
    mt_array_of(float) vertices;
    mt_array_of(uint32_t) indices;
    int attribute_count;
};

void mt_mesh_free(mt_mesh *mesh);

#endif
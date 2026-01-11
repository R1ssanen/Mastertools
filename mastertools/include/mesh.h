#ifndef MASTERTOOLS_MESH_H_
#define MASTERTOOLS_MESH_H_

#include "types.h"

typedef struct mt_mesh mt_mesh;
struct mt_mesh
{
    float *vertices;
    size_t vertex_count;
    uint *indices;
    size_t index_count;
    uint attribute_count;
};

#endif
#ifndef MASTERTOOLS_MESH_H_
#define MASTERTOOLS_MESH_H_

#include "types.h"

typedef struct mt_mesh mt_mesh;
struct mt_mesh
{
    float *vertices;
    int *indices;
    size_t vertex_count;
    size_t index_count;
    int attribute_count;
};

#endif
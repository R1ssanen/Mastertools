#ifndef MASTERTOOLS_MESH_H_
#define MASTERTOOLS_MESH_H_

#include <string.h>

#include "types.h"
#include "utility/array.h"

typedef struct mt_mesh mt_mesh;
struct mt_mesh
{
    mt_array vertices; // float
    mt_array indices;  // int
    uint attribute_count;
};

void mt_mesh_free(mt_mesh *mesh);

#endif
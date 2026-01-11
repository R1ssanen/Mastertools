#include "scene/mesh.h"

void mt_mesh_free(mt_mesh *mesh)
{
    mt_array_free(&mesh->vertices);
    mt_array_free(&mesh->indices);

#if defined(MT_SANITIZE_FREE)
    memset(mesh, 0, sizeof(*mesh));
#endif
}

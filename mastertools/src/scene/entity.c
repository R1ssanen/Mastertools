#include "scene/entity.h"

#include <immintrin.h>
#include <string.h>

#include "cJSON.h"
#include "loaders/wavefront_obj.h"
#include "logging.h"
#include "system/library.h"
#include "utility/file.h"

void mt_entity_free(mt_entity *entity)
{
    mt_array_foreach(&entity->meshes, mt_mesh, mesh)
    {
        mt_mesh_free(mesh);
    }

    mt_array_free(&entity->meshes);

    entity->desc->destroy(entity->desc);
    _mm_free(entity->shader_instance);

    mt_library_free(&entity->shader_lib);
    free(entity);
}

mt_entity *parse_node_entity_json(const cJSON *object)
{
    const cJSON *file = cJSON_GetObjectItemCaseSensitive(object, "file");
    if (!file || !cJSON_IsString(file))
    {
        LERROR("Entity must have a valid file associated");
        return NULL;
    }

    mt_string_view path = mt_string_refer_raw(file->valuestring);

    mt_string extension;
    if (!mt_file_get_extension(path, &extension))
    {
        LERROR("No extension");
        return NULL;
    }

    if (mt_string_equal_sr(extension, ".obj"))
    {
        mt_string_free(&extension);
        LINFO("Loading OBJ file '%s'", path.str);
    }
    else
    {
        mt_string_free(&extension);
        LERROR("Unsupported object file format");
        return NULL;
    }

    mt_entity *entity = mt_load_wavefront_obj(path);

    cJSON *shader = cJSON_GetObjectItemCaseSensitive(object, "shader");
    if (!shader)
    {
        LERROR("Entity must have a shader");
        return NULL;
    }

    mt_string_view shader_path = mt_string_refer_raw(shader->valuestring);
    if (!mt_library_load(shader_path, &entity->shader_lib))
    {
        LERROR("Failed to load shader library '%s'", shader_path.str);
    }

    void *get_spec = mt_library_load_symbol(&entity->shader_lib, mt_string_refer_raw("get_description"));
    entity->desc = ((rohan_get_description_fn)get_spec)();
    entity->shader_instance = _mm_malloc(entity->desc->instance_size, 32);

    return entity;
}

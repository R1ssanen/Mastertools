#include "scene/entity.h"

#include <string.h>

#include "cJSON.h"
#include "loaders/wavefront_obj.h"
#include "logging.h"
#include "utility/file.h"

//
#include "system/library.h"

void mt_entity_free(mt_entity *entity)
{
    mt_array_foreach(&entity->meshes, mt_mesh, mesh)
    {
        mt_mesh_free(mesh);
    }

    mt_array_free(&entity->meshes);

    entity->shader.destroy(&entity->shader);
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

    rohan_shader_instance_fn get_instance =
        (rohan_shader_instance_fn)mt_library_load_symbol(&entity->shader_lib, mt_string_refer_raw("create_instance"));
    entity->shader = get_instance();

    return entity;
}

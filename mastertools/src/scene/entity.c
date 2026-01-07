#include "scene/scene.h"

#include "allocator.h"
#include "cJSON.h"
#include "loaders/wavefront_obj.h"
#include "logging.h"

void *parse_node_entity_json(struct mt_allocator *alloc, const cJSON *object)
{
    cJSON *file = cJSON_GetObjectItemCaseSensitive(object, "file");
    if (!file || !cJSON_IsString(file))
    {
        LERROR("Entity must have a valid file associated");
        return NULL;
    }

    return load_entity_wavefront_obj(alloc, file->valuestring);
}

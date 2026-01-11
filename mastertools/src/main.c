
#include "engine.h"
#include "logging.h"

#include "scene/node.h"
#include "utility/file.h"
#include "utility/mstring.h"

#include <stdlib.h>
#include <string.h>

#include "rohan.h"

int main(void)
{
    // if (argc == 1)
    // {
    //     LINFO("No map file provided.");
    //     return 0;
    // }

    mt_string_view scene_path = mt_string_refer_raw("resource/test_scene.json");

    int status = 0;
    mt_engine engine;

    // rohan_render_triangle_raw(NULL, 5, 34, 1234, 523, 67, 84, 214, NULL, NULL, NULL);

    if (!mt_engine_create(scene_path, &engine))
    {
        status = 1;
    }
    else
    {
        status = mt_engine_run(&engine);
    }

    mt_engine_free(&engine);
    return status;
}

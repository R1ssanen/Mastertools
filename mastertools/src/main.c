#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "logging.h"

#include "scene/node.h"
#include "utility/file.h"
#include "utility/mtstring.h"

#include "rohan.h"
#include "utility/array.h"

int main(int argc, char *argv[])
{
    rohan_init();

    mt_string_view scene_path;
    if (argc < 2)
    {
        LINFO("No map file provided, loading default.");
        scene_path = mt_string_refer_raw("resource/test_scene.json");
    }
    else
    {
        scene_path = mt_string_refer_raw(argv[1]);
    }

    int status = 0;
    mt_engine engine;
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

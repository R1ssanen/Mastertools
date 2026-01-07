
#include "engine.h"
#include "logging.h"

#include "scene/node.h"
#include "utility/mstring.h"

int main(int argc, char *argv[])
{
    // if (argc == 1)
    // {
    //     LINFO("No map file provided.");
    //     return 0;
    // }

    argv[1] = "scenes/test.json";

    int status = 0;
    mt_engine engine;

    if (!mt_engine_create(mt_mstring_copy_raw(argv[1]), &engine))
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

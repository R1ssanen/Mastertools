#include "engine.h"
#include "logging.h"
#include "map/map.h"
#include "utility/mtstring.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        LINFO("Usage: mastertools.exe [map file]");
        return 0;
    }

    mt_engine engine;
    if (!mt_engine_create(&engine))
    {
        return 1;
    }

    if (!mt_map_load(mt_string_refer_raw(argv[1]), &engine.map))
    {
        mt_engine_free(&engine);
        return 1;
    }

    if (!mt_engine_run(&engine))
    {
        mt_engine_free(&engine);
        return 1;
    }

    mt_engine_free(&engine);
    return 0;
}

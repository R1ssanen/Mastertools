cmake_minimum_required(VERSION 3.10)
project(mastertools VERSION 2026.1.0 LANGUAGES C)

add_executable(mastertools
	"mastertools/src/main.c"
    "mastertools/src/engine.c"

    "mastertools/src/utility/mstring.c"
    "mastertools/src/utility/file.c"
    "mastertools/src/utility/array.c"

    "mastertools/src/loaders/wavefront_obj.c"

    "mastertools/src/scene/scene.c"
    "mastertools/src/scene/entity.c"
    "mastertools/src/scene/mesh.c"
    "mastertools/src/scene/node.c"

    "mastertools/src/backend/windows/library.c"
    "mastertools/src/backend/sdl/window.c"

    "vendor/cJSON.c"
)

target_include_directories(mastertools PRIVATE "mastertools/include" "vendor")

target_compile_definitions(mastertools PRIVATE MT_ANSI_COLORS MT_SANITIZE_FREE)

set_target_properties (mastertools PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED TRUE
	C_EXTENSIONS FALSE
)

target_compile_options(mastertools PRIVATE
    -Wall -Wextra -Wpedantic -march=native -mavx2 -mfma
    $<$<CONFIG:Debug>: -g -O0 -fno-inline -fno-omit-frame-pointer>
    $<$<CONFIG:Release>: -O3>
)

find_package(SDL3 REQUIRED)
add_subdirectory("vendor/rohan" EXCLUDE_FROM_ALL)

target_link_libraries(mastertools PRIVATE m SDL3::SDL3 rohan)

add_custom_command(TARGET mastertools
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:rohan> ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory("test_shader")
add_subdirectory("test_shader_2")

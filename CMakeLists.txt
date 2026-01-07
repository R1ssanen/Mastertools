cmake_minimum_required(VERSION 3.10)
project(mastertools VERSION 2026.1.0 LANGUAGES C)

add_executable(mastertools)
target_sources(mastertools PRIVATE
	"mastertools/src/main.c"
    "mastertools/src/allocator.c"
    "mastertools/src/engine.c"
    "mastertools/src/scene/scene.c"
    "mastertools/src/scene/entity.c"
    "mastertools/src/utility/mstring.c"
    "mastertools/src/utility/file.c"

    "mastertools/src/backend/windows/library.c"
    "mastertools/src/backend/sdl_window.c"

    "vendor/cJSON.c"
)

target_include_directories(mastertools PRIVATE "mastertools/include" "vendor")

set_target_properties (mastertools PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED TRUE
	C_EXTENSIONS FALSE
)

target_compile_options(mastertools PRIVATE
    -Wall -Wextra -Wpedantic -march=native -mavx2 -mfma
    $<$<CONFIG:Debug>: -g -O0>
    $<$<CONFIG:Release>: -O3>
)

target_compile_definitions(mastertools PRIVATE MT_ANSI_COLORS)

find_package(SDL3 REQUIRED)
add_subdirectory("vendor/rohan" EXCLUDE_FROM_ALL)

target_link_libraries(mastertools PRIVATE m SDL3::SDL3 rohan)

# add_custom_command (TARGET mastertools
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL3::SDL3> ${CMAKE_CURRENT_BINARY_DIR}
# )

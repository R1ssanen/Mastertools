cmake_minimum_required (VERSION 4.0)
project ("Mastertools"
	VERSION 2025.1.0
	LANGUAGES CXX
	DESCRIPTION "Software renderer"
)

add_executable (${PROJECT_NAME}
	"src/main.cpp"
	"src/camera.cpp"
	"src/plane.cpp"
	"src/loader.cpp"
	"src/clipping.cpp"
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set (OPTIMIZATIONS "-g")
else ()
	set (OPTIMIZATIONS "-O2")
endif ()

target_include_directories (${PROJECT_NAME} PRIVATE "include")
target_compile_options (${PROJECT_NAME} PRIVATE ${OPTIMIZATIONS} -Wall -Wextra -Wpedantic)

set_target_properties (${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	EXPORT_COMPILE_COMMANDS TRUE
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED TRUE
	CXX_EXTENSIONS FALSE
)

# external dependencies
add_subdirectory ("vendor/SDL" EXCLUDE_FROM_ALL)
add_subdirectory ("vendor/glm" EXCLUDE_FROM_ALL)

target_link_libraries (${PROJECT_NAME} PRIVATE glm::glm SDL3::SDL3)

add_custom_command (TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL3::SDL3> ${CMAKE_CURRENT_BINARY_DIR}
)

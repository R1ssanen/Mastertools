cmake_minimum_required(VERSION 3.30)
project(
  mtools
  VERSION 24.3.0
  DESCRIPTION "Software Rendering Engine"
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(
  ${PROJECT_NAME}
  "src/core/entry.cpp"
  "src/render/context.cpp"
  "src/core/application.cpp"
  "src/render/render.cpp"
  "src/core/camera.cpp"
  "src/resource/mesh.cpp"
  "src/resource/object.cpp"
  "src/resource/texture.cpp"
  "src/core/clipping.cpp"
  "src/core/color.cpp"
  "src/core/skybox.cpp"
  "src/core/callbacks.cpp"
  "src/core/light.cpp"
  "src/core/settings.cpp"
  "src/resource/scene.cpp"
  "src/core/timer.cpp"
  "src/core/emitter.cpp"
  "src/core/triangle.cpp"
  "src/core/uuid.cpp"
  "src/math/intersection.cpp"
  "src/math/plane.cpp"
  "src/math/aabb.cpp"
  "src/math/ray.cpp"
  "src/math/octree.cpp")

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED TRUE
             CXX_SCAN_FOR_MODULES FALSE
             LINKER_LANGUAGE CXX
             VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PRIVATE "src/" "ext/")
target_precompile_headers(${PROJECT_NAME} PRIVATE "src/mtpch.hpp")

set(BASE_COMPILE_FLAGS -Wall -Wextra -Wpedantic -march=native -flto=auto)
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${PROJECT_NAME} PRIVATE -g ${BASE_COMPILE_FLAGS})
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -O3 ${BASE_COMPILE_FLAGS})
endif()

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)

cmake_minimum_required(VERSION 3.28)
project(mtools VERSION 24.2.0 DESCRIPTION "Software Rendering Engine" LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  "src/entry.cpp"
  "src/context.cpp"
  "src/application.cpp"
  "src/render.cpp"
  "src/camera.cpp"
  "src/mesh.cpp"
  "src/object.cpp"
  "src/texture.cpp"
  "src/clipping.cpp"
  "src/color.cpp"
  "src/skybox.cpp"
  "src/callbacks.cpp"
  "src/light.cpp"
  "src/settings.cpp"
  "src/scene.cpp"
  "src/timer.cpp"
  "src/emitter.cpp"
  "src/triangle.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED TRUE
  CXX_SCAN_FOR_MODULES FALSE 
  VERSION ${PROJECT_VERSION}
)

target_include_directories(${PROJECT_NAME} PRIVATE "src/" "ext/")
target_precompile_headers(${PROJECT_NAME} PUBLIC "src/mtpch.hpp")

add_subdirectory("ext/glm/")
add_subdirectory("ext/sdl2/")

target_link_libraries(${PROJECT_NAME} PRIVATE glm SDL2 SDL2main)

SET (BASE_COMPILE_FLAGS -Wall -Wextra -Wpedantic -lphread -march=native -flto=auto) 
IF (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE EQUAL Debug)
  target_compile_options(${PROJECT_NAME} PRIVATE -g ${BASE_COMPILE_FLAGS})
ELSE ()
  target_compile_options(${PROJECT_NAME} PRIVATE -O3 ${BASE_COMPILE_FLAGS})
ENDIF ()

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
  VERBATIM
)


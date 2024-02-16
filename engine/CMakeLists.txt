cmake_minimum_required(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
project(Mastertools VERSION 24.1.0 DESCRIPTION "Software Rendering Engine")

add_executable(${PROJECT_NAME}
  entry.cpp
  src/context.cpp
  src/application.cpp
  src/render.cpp
  src/camera.cpp
  src/mesh.cpp
  src/object.cpp
  src/texture.cpp
  src/clipping.cpp
  src/color.cpp
  src/skybox.cpp
  src/callbacks.cpp
  src/light.cpp
  src/settings.cpp
  src/scene.cpp
  src/timer.cpp
  src/emitter.cpp
)

target_precompile_headers(${PROJECT_NAME} PUBLIC "deps/srpch.hpp")

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED TRUE
  VERSION ${PROJECT_VERSION}
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BUILD_TYPE}/"
)

target_include_directories(${PROJECT_NAME} PRIVATE src/ deps/)

find_library(SDL SDL2 NAMES SDL2 libSDL2)
find_library(SDL_FONT SDL2_ttf NAMES SDL2_ttf libSDL2_ttf)

find_package(OpenMP)
IF(OpenMP_CXX_FOUND)
  target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
ENDIF()

IF(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE EQUAL Debug)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pg -g -flto=auto -lphread -D_GLIBCXX_PARALLEL -march=native -mavx2)
ELSE()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -pg -O3 -flto=auto -march=native -mavx2) # -fomit-frame-pointer
ENDIF()

target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL})


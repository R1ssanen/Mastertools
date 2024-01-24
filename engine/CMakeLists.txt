cmake_minimum_required(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
project(Mastertools VERSION 24.1.0 DESCRIPTION "Software Renderer Engine")

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
  src/culling.cpp
  src/color.cpp
  src/skybox.cpp
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
find_library(SDL_IMG SDL2_image NAMES SDL2_image libSDL2_image)

# find_library(OPENAL openal NAMES openal libopenal)
# find_library(SNDFILE libsndfile NAMES sndfile libsndfile)
# ${OPENAL} ${SNDFILE}
target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL} ${SDL_IMG})

IF(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE EQUAL Debug)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g -flto=auto -pg -lpthread)
ELSE()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -O2 -flto=auto -pg -lpthread)
ENDIF()

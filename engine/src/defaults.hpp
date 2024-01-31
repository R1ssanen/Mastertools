#pragma once

#include "mesh.hpp"
#include "texture.hpp"

namespace {
core::mesh_t s_DefaultLightOverlay;
core::texture_t s_DefaultTexture;
}  // namespace

namespace core {

void InitDefaults();

core::mesh_t GetDefaultLightOverlay();
core::texture_t GetDefaultTexture();

}  // namespace core

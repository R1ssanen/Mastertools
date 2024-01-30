#pragma once

#include "mesh.hpp"
#include "texture.hpp"

namespace {
core::mesh_t s_DefaultLightOverlay;
core::texture_t s_DefaultTexture;
}  // namespace

namespace core {

void InitDefaults();

const core::mesh_t& GetDefaultLightOverlay();

}  // namespace core

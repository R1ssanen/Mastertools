#include "defaults.hpp"

namespace core {

void InitDefaults() {
  s_DefaultLightOverlay =
      core::LoadMeshOBJ("../../engine/builtins/", "light.obj", false)[0];
  s_DefaultTexture =
      core::NewTexture("../../engine/builtins/untextured.png", true, true);
}

const mesh_t& GetDefaultLightOverlay() {
  return s_DefaultLightOverlay;
}

const texture_t& GetDefaultTexture() {
  return s_DefaultTexture;
}

}  // namespace core

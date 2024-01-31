#include "defaults.hpp"

namespace core {

void InitDefaults() {
  s_DefaultLightOverlay =
      core::LoadMeshOBJ("../../engine/builtins/", "light.obj", false)[0];
  s_DefaultTexture = core::MipmapTexture::New(
      "../../engine/builtins/untextured.png", 6, false, true);
}

mesh_t GetDefaultLightOverlay() {
  return s_DefaultLightOverlay;
}

texture_t GetDefaultTexture() {
  return s_DefaultTexture;
}

}  // namespace core

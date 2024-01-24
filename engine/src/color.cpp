#include "color.hpp"

#include "srpch.hpp"

namespace core {

uint32_t ToUint32(float r, float g, float b, float a) {
  glm::u8vec4 Color{r * 255, g * 255, b * 255, a * 255};
  return glm::packUint4x8(Color);
}

uint32_t ToUint32(int r, int g, int b, int a) {
  glm::u8vec4 Color{r, g, b, a};
  return glm::packUint4x8(Color);
}

uint32_t ToUint32(const glm::fvec3& t_Color) {
  return glm::packUint4x8(glm::u8vec4(t_Color * 255.f, 1.f));
}

uint32_t ToUint32(const glm::fvec4& t_Color) {
  return glm::packUint4x8(t_Color * 255.f);
}

std::array<uint8_t, 4> UnpackUint32(const uint32_t& t_Color) {
  glm::u8vec4 Color{glm::unpackUint4x8(t_Color)};
  return std::array<uint8_t, 4>{Color.x, Color.y, Color.z, Color.w};
}

float AsFloat(const uint32_t& t_Color, Part t_ColorMap) {
  return static_cast<float>(t_Color & static_cast<uint32_t>(t_ColorMap)) /
         255.f;
}

uint8_t AsUint8(const uint32_t& t_Color, Part t_ColorMap) {
  return t_Color & static_cast<uint32_t>(t_ColorMap);
}

}  // namespace core

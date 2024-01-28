#include "color.hpp"

#include "srpch.hpp"

namespace core {

uint32_t ToUint32(float r, float g, float b, float a) {
  glm::u8vec4 Color{a * 255, g * 255, b * 255, r * 255};
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

float AsFloat(const uint32_t& t_Color, ColorPart t_ColorMask) {
  return (t_Color & static_cast<uint32_t>(t_ColorMask)) / 255.f;
}

uint8_t AsUint8(const uint32_t& t_Color, ColorPart t_ColorMask) {
  return t_Color & static_cast<uint32_t>(t_ColorMask);
}

uint32_t BlendUint32(const uint32_t& t_ColorA,
                     const uint32_t& t_ColorB,
                     float t_Scalar) {
  glm::fvec4 ColorA{glm::unpackUint4x8(t_ColorA)},
      ColorB{glm::unpackUint4x8(t_ColorB)};

  return glm::packUint4x8(ColorA * (1.f - t_Scalar) + ColorB * t_Scalar);
}

uint32_t AverageUint32(const uint32_t& t_ColorA, const uint32_t& t_ColorB) {
  glm::fvec4 ColorA{glm::unpackUint4x8(t_ColorA)},
      ColorB{glm::unpackUint4x8(t_ColorB)};

  return glm::packUint4x8((ColorA + ColorB) * 0.5f);
}

uint32_t ModRGB(const uint32_t& t_Color, float t_Scalar) {
  glm::fvec4 Color{glm::unpackUint4x8(t_Color)};
  return glm::packUint4x8(Color * t_Scalar);
}

}  // namespace core

#pragma once

#include "srpch.hpp"

namespace core {

enum class Part : uint32_t {
  RED = 0x00FF0000,
  GREEN = 0x0000FF00,
  BLUE = 0x000000FF,
  ALPHA = 0xFF000000
};

uint32_t ToUint32(float r, float g, float b, float a = 1.f);
uint32_t ToUint32(const glm::fvec3& vec);
uint32_t ToUint32(const glm::fvec4& vec);

std::array<uint8_t, 4> UnpackUint32(const uint32_t& t_Color);
inline glm::vec3 Uint32ToVec3(const uint32_t& t_Color) {
  auto [r, g, b, a] = UnpackUint32(t_Color);
  return glm::vec3(r, g, b);
}

float AsFloat(const uint32_t& t_Color, Part t_ColorMask);
uint8_t AsUint8(const uint32_t& t_Color, Part t_ColorMask);

uint32_t BlendUint32(const uint32_t& t_ColorA,
                     const uint32_t& t_ColorB,
                     float t_Scalar);
uint32_t AverageUint32(const uint32_t& t_ColorA, const uint32_t& t_ColorB);

uint32_t ModUint32(const uint32_t& t_Color, float t_Scalar);

}  // namespace core

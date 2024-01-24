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

float AsFloat(const uint32_t& t_Color, Part t_ColorMask);
uint8_t AsUint8(const uint32_t& t_Color, Part t_ColorMask);

}  // namespace core

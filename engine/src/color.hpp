#pragma once

#include "srpch.hpp"

#define INVERSE_MAX_UINT8 1.f / 255.f

namespace core {

enum class Channel : uint8_t { RED = 24, GREEN = 16, BLUE = 8, ALPHA = 0 };

uint32_t ToUint32(float r, float g, float b, float a = 1.f);
uint32_t ToUint32(const glm::vec4& t_Color);

std::array<uint8_t, 4> UnpackToArray(const uint32_t& t_Color);
glm::vec4 UnpackToVec4(const uint32_t& t_Color);
uint8_t Get(const uint32_t& t_Color, Channel t_ColorChannel);

uint32_t BlendUint32(const uint32_t& A, const uint32_t& B, float t_Scalar);
uint32_t AverageUint32(const uint32_t& A, const uint32_t& B);
uint32_t ModUint32(const uint32_t& t_Color, float t_Scalar);

}  // namespace core

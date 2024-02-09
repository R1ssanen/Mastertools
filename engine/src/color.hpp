#pragma once

#include "srpch.hpp"

#define INVERSE_MAX_UINT8 1.f / 255.f

namespace core
{

enum class Channel : uint8_t
{
    RED = 24,
    GREEN = 16,
    BLUE = 8,
    ALPHA = 0
};

// packing

uint32_t ToUint32(float r, float g, float b, float a);
uint32_t ToUint32(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
uint32_t ToUint32(const glm::vec4& t_Color);

// unpacking

std::array<uint8_t, 4> UnpackToArray(uint32_t t_Color);
glm::vec4 UnpackToVec4(uint32_t t_Color);
uint8_t Get(uint32_t t_Color, Channel t_ColorChannel);

// operations

uint32_t BlendUint32(uint32_t A, uint32_t B, float t_Scalar);
uint32_t AverageUint32(uint32_t A, uint32_t B);
uint32_t ModUint32(uint32_t t_Color, float t_Scalar);

} // namespace core

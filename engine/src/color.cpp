#include "color.hpp"

#include <limits>

#include "srpch.hpp"

namespace core
{

// packing

uint32_t ToUint32(float r, float g, float b, float a)
{
    glm::u8vec4 Color{a * 255, b * 255, g * 255, r * 255};
    return glm::packUint4x8(Color);
}

uint32_t ToUint32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    glm::u8vec4 Color{a, b, g, r};
    return glm::packUint4x8(Color);
}

uint32_t ToUint32(const glm::vec4& t_Color)
{
    return glm::packUint4x8(glm::u8vec4(t_Color.a, t_Color.b, t_Color.g, t_Color.r));
}

// unpacking

std::array<uint8_t, 4> UnpackToArray(uint32_t t_Color)
{
    glm::u8vec4 Color{glm::unpackUint4x8(t_Color)};
    return std::array<uint8_t, 4>{Color.a, Color.b, Color.g, Color.r};
}

glm::vec4 UnpackToVec4(uint32_t t_Color)
{
    glm::u8vec4 Color{glm::unpackUint4x8(t_Color)};
    return glm::vec4(Color.a, Color.b, Color.g, Color.r);
}

uint8_t Get(uint32_t t_Color, Channel t_ColorChannel)
{
    return (t_Color >> static_cast<uint8_t>(t_ColorChannel)) & 0xFF;
}

// operations

uint32_t BlendUint32(uint32_t A, uint32_t B, float t_Scalar)
{
    glm::vec4 ColorA{UnpackToVec4(A)}, ColorB{UnpackToVec4(B)};
    return ToUint32(ColorA * t_Scalar + ColorB * (1.f - t_Scalar));
}

uint32_t AverageUint32(uint32_t A, uint32_t B)
{
    glm::vec4 ColorA{UnpackToVec4(A)}, ColorB{UnpackToVec4(B)};
    return ToUint32((ColorA + ColorB) * 0.5f);
}

uint32_t ModUint32(uint32_t t_Color, float t_Scalar)
{
    glm::vec4 Color{UnpackToVec4(t_Color)};
    return ToUint32(glm::vec4(Color.r * t_Scalar, Color.g * t_Scalar, Color.b * t_Scalar, Color.a));
}

float Luminosity(const glm::vec3& t_Color) {
    constexpr glm::vec3 Weights = {0.2126f, 0.7152f, 0.0722f};
    return glm::dot(t_Color, Weights);
}

uint32_t GetRandomColor() {
    float S = glm::linearRand<float>(0.f, 1.f);
    return static_cast<uint32_t>(std::numeric_limits<uint32_t>::max() * S);
}

} // namespace core

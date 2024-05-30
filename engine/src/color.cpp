#include "color.hpp"

#include "mtpch.hpp"

namespace core
{

// packing

u32 ToUint32(f32 R, f32 G, f32 B, f32 A)
{
    return glm::packUint4x8(glm::u8vec4{ A * 255, B * 255, G * 255, R * 255 });
}

u32 ToUint32(u8 R, u8 G, u8 B, u8 A)
{
    return glm::packUint4x8(glm::u8vec4{ A, B, G, R });
}


u32 ToUint32(const glm::u8vec4& RGBA)
{
    return glm::packUint4x8(glm::u8vec4{ RGBA.a, RGBA.b, RGBA.g, RGBA.r });
}

u32 ToUint32(const glm::vec4& RGBA) {
    return glm::packUint4x8(glm::u8vec4{ RGBA.a * 255, RGBA.b * 255, RGBA.g * 255, RGBA.r * 255 });
}

// unpacking

std::array<u8, 4> UnpackToArray(u32 RGBA)
{
    glm::u8vec4 Color = glm::unpackUint4x8(RGBA);
    return std::array<u8, 4>{Color.a, Color.b, Color.g, Color.r};
}

glm::vec4 UnpackToVec4(u32 RGBA) {
    glm::u8vec4 Color = glm::unpackUint4x8(RGBA);
    return glm::vec4(Color.a, Color.b, Color.g, Color.r);
}

u8 Get(u32 RGBA, Channel ColorChannel) {
    return (RGBA >> static_cast<u8>(ColorChannel)) & 0xFF;
}

// operations

u32 BlendUint32(u32 A, u32 B, f32 Scalar)
{
    const glm::vec4 ColorA = UnpackToVec4(A), ColorB = UnpackToVec4(B);
    return ToUint32(glm::u8vec4{ ColorA * Scalar + ColorB * (1.f - Scalar) });
}

u32 AverageUint32(u32 A, u32 B)
{
    const glm::vec4 ColorA = UnpackToVec4(A), ColorB = UnpackToVec4(B);
    return ToUint32(glm::u8vec4{ (ColorA + ColorB) * 0.5f });
}

u32 ModUint32(u32 RGBA, f32 Scalar) {
    const glm::vec4 Color = UnpackToVec4(RGBA);
    return ToUint32(glm::u8vec4{ Color.r * Scalar, Color.g * Scalar, Color.b * Scalar, Color.a });
}

f32 Luminosity(const glm::vec3& RGB) {
    constexpr glm::vec3 Weights = {0.2126f, 0.7152f, 0.0722f};
    return glm::dot(RGB, Weights);
}

u32 GetRandomColor() {
    const glm::vec4 RandScalars = glm::vec4((glm::sphericalRand(1.f) + 1.f) * 0.5f, 1.f);
    return ToUint32(RandScalars);
}

} // namespace core

#pragma once

#include "../mtpch.hpp"

#define INVERSE_MAX_UINT8 1.f / 255.f

namespace mt {

    enum class Channel : u8 { RED = 24, GREEN = 16, BLUE = 8, ALPHA = 0 };

    // packing

    u32 ToUint32(f32 R, f32 G, f32 B, f32 A);
    u32 ToUint32(u8 R, u8 G, u8 B, u8 A);
    u32 ToUint32(const glm::u8vec4& RGBA);
    u32 ToUint32(const glm::vec4& RGBA);

    // unpacking

    std::array<u8, 4> UnpackToArray(u32 RGBA);
    glm::vec4         UnpackToVec4(u32 RGBA);
    u8                Get(u32 RGBA, Channel ColorChannel);

    // operations

    u32 BlendUint32(u32 A, u32 B, f32 Scalar);
    u32 AverageUint32(u32 A, u32 B);
    u32 ModUint32(u32 RGBA, f32 Scalar);

    f32 Luminosity(const glm::vec3& RGB);
    u32 GetRandomColor();

} // namespace mt

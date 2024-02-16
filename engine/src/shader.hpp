#pragma once

#include "color.hpp"
#include "context.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "buffer.hpp"

namespace core
{

using shader_t = std::function<void(buffer_t, const texture_t&, const glm::vec4&, const glm::vec2&, float, void*)>;

inline void OpaqueSTD(buffer_t t_Buffer, const texture_t& t_Texture, const glm::vec4& t_Pos,
                      const glm::vec2& t_UV, float t_Light, void* t_ShaderData = nullptr)
{
    uint32_t Pixel = t_Texture->Sample(t_UV, t_Pos.z);

    uint32_t LightColor = *static_cast<uint32_t*>(t_ShaderData);

    unsigned Loc = static_cast<unsigned>(t_Pos.y * t_Buffer->GetWidth() + t_Pos.x);
    t_Buffer->SetPixel(Loc, BlendUint32(Pixel, LightColor, t_Light));
    t_Buffer->SetDepth(Loc, t_Pos.z);
}

inline void TransparentSTD(buffer_t t_Buffer, const texture_t& t_Texture, const glm::vec4& t_Pos,
                           const glm::vec2& t_UV, float t_Light, void* t_ShaderData = nullptr)
{
    uint32_t Pixel = t_Texture->Sample(t_UV, t_Pos.z);
    uint8_t Alpha = Get(Pixel, Channel::ALPHA);

    unsigned Loc = static_cast<unsigned>(t_Pos.y * t_Buffer->GetWidth() + t_Pos.x);

    switch (Alpha)
    {
    case 0:
        return;

    case 255:
        t_Buffer->SetPixel(Loc, ModUint32(Pixel, t_Light));
        t_Buffer->SetDepth(Loc, t_Pos.z);
        return;

    default:
        t_Buffer->SetPixel(Loc, BlendUint32(ModUint32(Pixel, t_Light), t_Buffer->GetPixel(Loc),
                                                 static_cast<float>(Alpha * INVERSE_MAX_UINT8)));
        return;
    }
}

inline void ParticleSTD(buffer_t t_Buffer, const texture_t& t_Texture, const glm::vec4& t_Pos,
                        const glm::vec2& t_UV, float t_Light, void* t_ShaderData = nullptr)
{
    uint32_t Pixel = t_Texture->Sample(t_UV, t_Pos.z);
    uint8_t Alpha = Get(Pixel, Channel::ALPHA);

    unsigned Loc = static_cast<unsigned>(t_Pos.y * t_Buffer->GetWidth() + t_Pos.x);
    float ParticleAlpha = *static_cast<float*>(t_ShaderData);

    switch (Alpha)
    {
    case 0:
        return;

    default:
        t_Buffer->SetPixel(Loc, BlendUint32(ModUint32(Pixel, t_Light), t_Buffer->GetPixel(Loc),
                                                 static_cast<float>(Alpha * INVERSE_MAX_UINT8)) * ParticleAlpha);
        return;
    }
}

} // namespace core

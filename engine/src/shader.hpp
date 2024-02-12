#pragma once

#include "color.hpp"
#include "context.hpp"
#include "srpch.hpp"
#include "texture.hpp"

namespace core
{

using shader_t = std::function<void(Context&, const texture_t&, const glm::vec4&, const glm::vec2&, float)>;

inline void OpaqueSTD(Context& t_Context, const texture_t& t_Texture, const glm::vec4& t_Pos, const glm::vec2& t_UV,
                      float t_Light)
{
    uint32_t Pixel = t_Texture->Sample(t_UV, t_Pos.z);

    unsigned Loc = static_cast<unsigned>(t_Pos.y * t_Context.GetWidth() + t_Pos.x);
    t_Context.ColorBuffer[Loc] = ModUint32(Pixel, t_Light);// * (1.f - t_Pos.z));
    t_Context.DepthBuffer[Loc] = t_Pos.z;
}

inline void TransparentSTD(Context& t_Context, const texture_t& t_Texture, const glm::vec4& t_Pos,
                           const glm::vec2& t_UV, float t_Light)
{
    uint32_t Pixel = t_Texture->Sample(t_UV, t_Pos.z);
    uint8_t Alpha = Get(Pixel, Channel::ALPHA);

    unsigned Loc = static_cast<unsigned>(t_Pos.y * t_Context.GetWidth() + t_Pos.x);

    switch (Alpha)
    {
    case 0:
        return;

    case 255:
        t_Context.ColorBuffer[Loc] = ModUint32(Pixel, t_Light);
        t_Context.DepthBuffer[Loc] = t_Pos.z;
        return;

    default:
        t_Context.ColorBuffer[Loc] = BlendUint32(ModUint32(Pixel, t_Light), t_Context.ColorBuffer[Loc],
                                                 static_cast<float>(Alpha * INVERSE_MAX_UINT8));
        return;
    }
}

} // namespace core

#pragma once

#include "color.hpp"
#include "context.hpp"
#include "srpch.hpp"
#include "texture.hpp"

namespace core {

using shader_t = std::function<void(Context&,
                                    const texture_t&,
                                    const glm::vec4&,
                                    const glm::vec2&,
                                    float)>;

inline void OpaqueSTD(Context& t_Context,
                      const texture_t& t_Texture,
                      const glm::vec4& t_Pos,
                      const glm::vec2& t_UV,
                      float t_Light) {
  uint32_t Pixel{t_Texture->Sample(t_UV)};

  unsigned int Loc{
      static_cast<unsigned int>(t_Pos.y * t_Context.GetWidth() + t_Pos.x)};
  t_Context.ColorBuffer[Loc] = ModRGB(Pixel, t_Light);
  t_Context.DepthBuffer[Loc] = t_Pos.z;
}

inline void TransparentSTD(Context& t_Context,
                           const texture_t& t_Texture,
                           const glm::vec4& t_Pos,
                           const glm::vec2& t_UV,
                           float t_Light) {
  uint32_t Pixel{t_Texture->Sample(t_UV)};
  uint8_t Alpha{AsUint8(Pixel, ColorPart::ALPHA)};

  unsigned int Loc{
      static_cast<unsigned int>(t_Pos.y * t_Context.GetWidth() + t_Pos.x)};

  switch (Alpha) {
    case 0:
      return;

    case 255:
      t_Context.TransparentBuffer[Loc] = ModRGB(Pixel, t_Light);
      t_Context.DepthBuffer[Loc] = t_Pos.z;
      return;

    default:
      t_Context.TransparentBuffer[Loc] = BlendUint32(
          ModRGB(Pixel, t_Light), t_Context.ColorBuffer[Loc], Alpha / 255.f);
      return;
  }
}

}  // namespace core

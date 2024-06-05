#pragma once

#include "../core/buffer.hpp"
#include "../core/color.hpp"
#include "../mtpch.hpp"
#include "../render/context.hpp"
#include "texture.hpp"

namespace mt {

    /*
    struct Shader {
        virtual ~Shader() = default;
        virtual void operator() (buffer_t, texture_t, const glm::vec4&, const glm::vec2&, f32, u32)
    const = 0;
    };

    using shader_t = std::unique_ptr<Shader>;

    struct Opaque : public Shader {
        void operator() (buffer_t Buffer, texture_t Texture, const glm::vec4& Pos, const glm::vec2&
    UV, f32 Light, u32 LightColor) const override {

            u32 Pixel = Texture->Sample(UV, Pos.z);
            u32 Loc = static_cast<u32>(Pos.y * Buffer->GetWidth() + Pos.x);

            Buffer->SetPixel(Loc, BlendUint32(Pixel, LightColor, Light));
            Buffer->SetDepth(Loc, Pos.z);
        }

        static shader_t New() { return std::make_unique<Opaque>(); }
    };

    struct Transparent : public Shader {
        void operator() (buffer_t Buffer, texture_t Texture, const glm::vec4& Pos, const glm::vec2&
    UV, f32 Light, u32 LightColor) const override {

            u32 Pixel = Texture->Sample(UV, Pos.z);
            u8 Alpha = Get(Pixel, Channel::ALPHA);
            u32 Loc = static_cast<u32>(Pos.y * Buffer->GetWidth() + Pos.x);

            switch (Alpha)
            {
            case 0:
                return;

            case 255:
                Buffer->SetPixel(Loc, BlendUint32(Pixel, LightColor, Light));
                Buffer->SetDepth(Loc, Pos.z);
                return;

            default:
                Buffer->SetPixel(Loc, BlendUint32(BlendUint32(Pixel, LightColor, Light),
    Buffer->GetPixel(Loc), static_cast<f32>(Alpha * INVERSE_MAX_UINT8))); return;
            }
        }

        static shader_t New() { return std::make_unique<Transparent>(); }
    };

    struct Transparent : public Shader {
        void operator() (buffer_t Buffer, texture_t Texture, const glm::vec4& Pos, const glm::vec2&
    UV, f32 Light, u32 LightColor) const override {

            u32 Pixel = Texture->Sample(UV, Pos.z);
            u8 Alpha = Get(Pixel, Channel::ALPHA);
            u32 Loc = static_cast<u32>(Pos.y * Buffer->GetWidth() + Pos.x);

            switch (Alpha)
            {
            case 0:
                return;

            case 255:
                Buffer->SetPixel(Loc, BlendUint32(Pixel, LightColor, Light));
                Buffer->SetDepth(Loc, Pos.z);
                return;

            default:
                Buffer->SetPixel(Loc, BlendUint32(BlendUint32(Pixel, LightColor, Light),
    Buffer->GetPixel(Loc), static_cast<f32>(Alpha * INVERSE_MAX_UINT8))); return;
            }
        }

        static shader_t New() { return std::make_unique<Transparent>(); }
    };
    */

    using shader_t = void (*)(buffer_t, texture_t, const glm::vec4&, const glm::vec2&, f32, f32);

    inline void           OpaqueSTD(
                  buffer_t Buffer, texture_t Texture, const glm::vec4& Pos, const glm::vec2& UV, f32 Light,
                  f32 Alpha
              ) {

        const u32 Pixel = Texture->Sample(UV, Pos.z);
        /* const u32 Loc   = std::clamp(
            static_cast<u32>(Pos.y * Buffer->GetWidth() + Pos.x), 0, Buffer->GetResolution() - 1
        );*/

        const u32 Loc   = static_cast<u32>(Pos.y * Buffer->GetWidth() + Pos.x);

        Buffer->SetPixel(Loc, ModUint32(Pixel, Light));
        Buffer->SetDepth(Loc, Pos.z);
    };

    inline void TransparentSTD(
        buffer_t Buffer, texture_t Texture, const glm::vec4& Pos, const glm::vec2& UV, f32 Light,
        f32 Alpha
    ) {

        u32 Pixel      = Texture->Sample(UV, Pos.z);
        u8  PixelAlpha = Get(Pixel, Channel::ALPHA) * Alpha;
        u32 Loc        = std::clamp(
            static_cast<i32>(Pos.y * Buffer->GetWidth() + Pos.x), 0, Buffer->GetResolution() - 1
        );

        switch (PixelAlpha) {
        case 0: return;

        case 255:
            Buffer->SetPixel(Loc, ModUint32(Pixel, Light));
            Buffer->SetDepth(Loc, Pos.z);
            return;

        default:
            Buffer->SetPixel(
                Loc, BlendUint32(
                         ModUint32(Pixel, Light), Buffer->GetPixel(Loc),
                         static_cast<f32>(Alpha * INVERSE_MAX_UINT8)
                     )
            );
            return;
        }
    };

    inline void ParticleSTD(
        buffer_t Buffer, texture_t Texture, const glm::vec4& Pos, const glm::vec2& UV, f32 Light,
        f32 Alpha
    ) {

        u32 Pixel      = Texture->Sample(UV, Alpha);
        u8  PixelAlpha = Get(Pixel, Channel::ALPHA) * Alpha;
        u32 Loc        = std::clamp(
            static_cast<i32>(Pos.y * Buffer->GetWidth() + Pos.x), 0, Buffer->GetResolution() - 1
        );

        switch (PixelAlpha) {
        case 0: return;

        case 255:
            Buffer->SetPixel(Loc, ModUint32(Pixel, Light));
            Buffer->SetDepth(Loc, Pos.z);
            return;

        default:
            Buffer->SetPixel(
                Loc, BlendUint32(
                         ModUint32(Pixel, Light), Buffer->GetPixel(Loc),
                         static_cast<f32>(Alpha * INVERSE_MAX_UINT8)
                     )
            );
            return;
        }
    };

} // namespace mt

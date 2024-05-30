#include "render.hpp"

#include <initializer_list>

#include "buffer.hpp"
#include "color.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "mtpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace {

    void RenderTriTop(core::buffer_t Buffer, core::Triangle& Tri, core::f32 InverseFar) {
        auto [a, b, c]          = Tri.m_Vertices;
        core::texture_t Texture = core::GetTexture(Tri.m_TextureID);
        core::shader_t  Shade   = Tri.m_Shader;

        if (b.m_Pos.x < a.m_Pos.x) { std::swap(b, a); }

        core::f32 InvHeight = 1.f / (c.m_Pos.y - a.m_Pos.y);
        core::f32 InvWidth  = 1.f / (b.m_Pos.x - a.m_Pos.x);

        glm::vec2 lx = { (c.m_Pos.x - a.m_Pos.x) * InvHeight, (c.m_Pos.x - b.m_Pos.x) * InvHeight };
        glm::vec2 lz = { (b.m_Pos.z - a.m_Pos.z) * InvWidth, (c.m_Pos.z - a.m_Pos.z) * InvHeight };
        glm::vec2 lw = { (b.m_Pos.w - a.m_Pos.w) * InvWidth, (c.m_Pos.w - a.m_Pos.w) * InvHeight };

        glm::vec2 lu = { (b.m_UV.x - a.m_UV.x) * InvWidth, (c.m_UV.x - a.m_UV.x) * InvHeight };
        glm::vec2 lv = { (b.m_UV.y - a.m_UV.y) * InvWidth, (c.m_UV.y - a.m_UV.y) * InvHeight };
        glm::vec2 ll = { (b.m_Light - a.m_Light) * InvWidth, (c.m_Light - a.m_Light) * InvHeight };

        core::i32 Row       = core::i32(std::ceil(a.m_Pos.y)) * Buffer->GetWidth();

        for (core::i32 y = 0; y < core::i32(std::ceil(c.m_Pos.y - a.m_Pos.y) - 1); y++) {
            core::i32 x0 = core::i32(std::ceil(a.m_Pos.x + y * lx.x) - 1);
            core::i32 x1 = core::i32(std::ceil(b.m_Pos.x + y * lx.y));

            core::f32 z0 = a.m_Pos.z + y * lz.y;
            core::f32 w0 = a.m_Pos.w + y * lw.y;
            core::f32 u0 = a.m_UV.x + y * lu.y;
            core::f32 v0 = a.m_UV.y + y * lv.y;
            core::f32 l0 = a.m_Light + y * ll.y;

            for (core::i32 x = x0; x <= x1; x++) {
                core::f32 w = 1.f / w0;
                core::f32 z = glm::clamp(z0 * w * InverseFar, 0.f, 1.f);

                if (Buffer->GetDepth(Row + x) > z) {
                    core::f32 Light =
                        glm::clamp(l0, core::GetSettingAmbientIntensity(), 1.f);

                    Shade(
                        Buffer, Texture, glm::vec4(x, y + static_cast<core::i32>(a.m_Pos.y), z, w),
                        glm::vec2(std::fabs(u0 * w), std::fabs(v0 * w)), Light, 0, 1.f
                    );
                    // Buffer->SetPixel(Row + x, Texture->Sample(glm::vec2(std::fabs(u0 * w),
                    // std::fabs(v0 * w)), z)); Buffer->SetDepth(Row + x, z);
                }

                else {
                    /* Buffer->SetPixel(
                        Row + x, core::BlendUint32(Buffer->GetPixel(Row + x), 0xFF0000FF, 0.5f)
                    );*/
                }

                z0 += lz.x;
                w0 += lw.x;
                u0 += lu.x;
                v0 += lv.x;
                l0 += ll.x;
            }

            Row += Buffer->GetWidth();
        }
    }

    void RenderTriBottom(core::buffer_t Buffer, core::Triangle& Tri, core::f32 InverseFar) {
        auto& [a, b, c]         = Tri.m_Vertices;
        core::texture_t Texture = core::GetTexture(Tri.m_TextureID);
        core::shader_t  Shade   = Tri.m_Shader;

        if (c.m_Pos.x < b.m_Pos.x) { std::swap(c, b); }

        core::f32 InvHeight = 1.f / (b.m_Pos.y - a.m_Pos.y);
        core::f32 InvWidth  = 1.f / (c.m_Pos.x - b.m_Pos.x);

        glm::vec2 lx = { (b.m_Pos.x - a.m_Pos.x) * InvHeight, (c.m_Pos.x - a.m_Pos.x) * InvHeight };
        glm::vec2 lz = { (c.m_Pos.z - b.m_Pos.z) * InvWidth, (b.m_Pos.z - a.m_Pos.z) * InvHeight };
        glm::vec2 lw = { (c.m_Pos.w - b.m_Pos.w) * InvWidth, (b.m_Pos.w - a.m_Pos.w) * InvHeight };

        glm::vec2 lu = { (c.m_UV.x - b.m_UV.x) * InvWidth, (b.m_UV.x - a.m_UV.x) * InvHeight };
        glm::vec2 lv = { (c.m_UV.y - b.m_UV.y) * InvWidth, (b.m_UV.y - a.m_UV.y) * InvHeight };
        glm::vec2 ll = { (c.m_Light - b.m_Light) * InvWidth, (b.m_Light - a.m_Light) * InvHeight };

        core::i32 Row       = core::i32(std::ceil(a.m_Pos.y)) * Buffer->GetWidth();

        for (core::i32 y = 0; y <= core::i32(std::ceil(b.m_Pos.y - a.m_Pos.y) - 1); y++) {
            core::i32 x0 = core::i32(std::ceil(a.m_Pos.x + y * lx.x) - 1);
            core::i32 x1 = core::i32(std::ceil(a.m_Pos.x + y * lx.y));

            core::f32 z0 = a.m_Pos.z + y * lz.y;
            core::f32 w0 = a.m_Pos.w + y * lw.y;
            core::f32 u0 = a.m_UV.x + y * lu.y;
            core::f32 v0 = a.m_UV.y + y * lv.y;
            core::f32 l0 = a.m_Light + y * ll.y;

            for (core::i32 x = x0; x <= x1; x++) {
                core::f32 w = 1.f / w0;
                core::f32 z = glm::clamp(z0 * w * InverseFar, 0.f, 1.f);

                if (Buffer->GetDepth(Row + x) > z) {
                    core::f32 Light =
                        glm::clamp(l0, core::GetSettingAmbientIntensity(), 1.f);

                    Shade(
                        Buffer, Texture, glm::vec4(x, y + static_cast<core::i32>(a.m_Pos.y), z, w),
                        glm::vec2(std::fabs(u0 * w), std::fabs(v0 * w)), Light, 0, 1.f
                    );
                    // Buffer->SetPixel(Row + x, Texture->Sample(glm::vec2(std::fabs(u0 * w),
                    // std::fabs(v0 * w)), z)); Buffer->SetDepth(Row + x, z);
                }

                else {
                    /* Buffer->SetPixel(
                        Row + x, core::BlendUint32(Buffer->GetPixel(Row + x), 0xFF0000FF, 0.5f)
                    );*/
                }

                z0 += lz.x;
                w0 += lw.x;
                u0 += lu.x;
                v0 += lv.x;
                l0 += ll.x;
            }

            Row += Buffer->GetWidth();
        }
    }

} // namespace

namespace core {

    void DrawWireframe(
        Context& Context, const Triangle& Tri, u8 r, u8 g, u8 b, u8 a
    ) {
        glm::vec2 Ratio      = GetResolutionRatio();
        const auto [A, B, C] = Tri.m_Vertices;

        SDL_FPoint Points[4] = {
            { A.m_Pos.x * Ratio.x, A.m_Pos.y * Ratio.y },
            { B.m_Pos.x * Ratio.x, B.m_Pos.y * Ratio.y },
            { C.m_Pos.x * Ratio.x, C.m_Pos.y * Ratio.y }
        };
        Points[3]          = Points[0];

        SDL_FRect Rects[3] = {
            { Points[0].x - 2.f, Points[0].y - 2.f, 4.f, 4.f },
            { Points[1].x - 2.f, Points[1].y - 2.f, 4.f, 4.f },
            { Points[2].x - 2.f, Points[2].y - 2.f, 4.f, 4.f }
        };

        SDL_SetRenderDrawColor(Context.Renderer, r, g, b, a);
        SDL_RenderDrawLinesF(Context.Renderer, Points, 4);

        SDL_SetRenderDrawColor(Context.Renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRectsF(Context.Renderer, Rects, 3);
    }

    void RenderTriBary(buffer_t Buffer, Triangle& Tri, f32 InverseFar) {
        auto [Min, Max] = Tri.GetBoundingBox();
        Min             = glm::clamp(
            Min, glm::vec2(0.f), glm::vec2(Buffer->GetWidth() - 1, Buffer->GetHeight() - 1)
        );
        Max = glm::clamp(
            Max, glm::vec2(0.f), glm::vec2(Buffer->GetWidth() - 1, Buffer->GetHeight() - 1)
        );

        if ((Max.x - Min.x) * (Max.y - Min.y) == 0.f) {
            return; // zero area triangle
        }

        auto [a, b, c]    = Tri.m_Vertices;
        texture_t Texture = GetTexture(Tri.m_TextureID);
        auto      Shade   = Tri.m_Shader;

        f32     InverseDeterminant =
            1.f / Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, c.m_Pos.x, c.m_Pos.y);

        for (i32 y = Min.y; y <= Max.y; y++) {
            i32  Row     = y * Buffer->GetWidth();
            b8 Outside = true;

            for (i32 x = Min.x; x <= Max.x; x++) {
                f32 B0 = Edge(b.m_Pos.x, b.m_Pos.y, c.m_Pos.x, c.m_Pos.y, x, y),
                      B1 = Edge(c.m_Pos.x, c.m_Pos.y, a.m_Pos.x, a.m_Pos.y, x, y),
                      B2 = Edge(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, x, y);

                if (B0 < 0.f && B1 < 0.f && B2 < 0.f) {
                    Outside = false;

                    B0 *= InverseDeterminant;
                    B1 *= InverseDeterminant;
                    B2 *= InverseDeterminant;

                    f32 w = 1.f / (a.m_Pos.w * B0 + b.m_Pos.w * B1 + c.m_Pos.w * B2);
                    f32 z = glm::clamp(
                        (a.m_Pos.z * B0 + b.m_Pos.z * B1 + c.m_Pos.z * B2) * w * InverseFar, 0.f,
                        1.f
                    );

                    if (Buffer->GetDepth(Row + x) <= z) {
                        // Buffer->SetPixel(Row + x, BlendUint32(Buffer->GetPixel(Row + x),
                        // 0xFF0000FF, 0.5f));
                        continue;
                    }

                    f32 u     = std::fabs(a.m_UV.x * B0 + b.m_UV.x * B1 + c.m_UV.x * B2) * w;
                    f32 v     = std::fabs(a.m_UV.y * B0 + b.m_UV.y * B1 + c.m_UV.y * B2) * w;

                    f32 Light = glm::clamp(
                        a.m_Light * B0 + b.m_Light * B1 + c.m_Light * B2,
                        GetSettingAmbientIntensity(), 1.f
                    );

                    u8 R = (a.m_LightColor.r * B0 + b.m_LightColor.r * B1 +
                                 c.m_LightColor.r * B2) *
                                255,
                            G = (a.m_LightColor.g * B0 + b.m_LightColor.g * B1 +
                                 c.m_LightColor.g * B2) *
                                255,
                            B = (a.m_LightColor.b * B0 + b.m_LightColor.b * B1 +
                                 c.m_LightColor.b * B2) *
                                255;

                    Shade(
                        Buffer, Texture, glm::vec4(x, y, z, w), glm::vec2(u, v), Light,
                        ToUint32(R, G, B, 255), Tri.m_Alpha
                    );
                }

                else {
                    if (!Outside) { break; }
                }
            }
        }
    }

    void RenderTriScan(buffer_t Buffer, Triangle& Tri, f32 InverseFar) {
        auto& [a, b, c] = Tri.m_Vertices;

        if (a.m_Pos.y > b.m_Pos.y) { std::swap(a, b); }
        if (b.m_Pos.y > c.m_Pos.y) { std::swap(b, c); }
        if (a.m_Pos.y > b.m_Pos.y) { std::swap(a, b); }

        if (std::trunc(a.m_Pos.y) == std::trunc(b.m_Pos.y)) {
            return RenderTriTop(Buffer, Tri, InverseFar);
        }

        if (std::trunc(b.m_Pos.y) == std::trunc(c.m_Pos.y)) {
            return RenderTriBottom(Buffer, Tri, InverseFar);
        }

        f32  LengthAtoD = (b.m_Pos.y - a.m_Pos.y) / (c.m_Pos.y - a.m_Pos.y);

        Vertex d          = {
                     .m_Pos        = a.m_Pos + (c.m_Pos - a.m_Pos) * LengthAtoD,
                     .m_Normal     = glm::normalize(a.m_Normal + (c.m_Normal - a.m_Normal) * LengthAtoD),
                     .m_LightColor = a.m_LightColor + (c.m_LightColor - a.m_LightColor) * LengthAtoD,
                     .m_UV         = a.m_UV + (c.m_UV - a.m_UV) * LengthAtoD,
                     .m_Light      = glm::clamp(a.m_Light + (c.m_Light - a.m_Light) * LengthAtoD, 0.f, 1.f)
        };

        Triangle Top = Tri, Bottom = Tri;
        Top.m_Vertices    = { b, d, c };
        Bottom.m_Vertices = { a, d, b };

        RenderTriTop(Buffer, Top, InverseFar);
        RenderTriBottom(Buffer, Bottom, InverseFar);
    }

} // namespace core

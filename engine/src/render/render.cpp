#include "render.hpp"

#include "../accel/accel.hpp"
#include "../core/buffer.hpp"
#include "../core/color.hpp"
#include "../core/triangle.hpp"
#include "../core/vertex.hpp"
#include "../mtpch.hpp"
#include "../resource/texture.hpp"
#include "context.hpp"

static constexpr mt::f32 s_BayerMatrix[16] = { 0.f / 16.f,  8.f / 16.f,  2.f / 16.f,  10.f / 16.f,
                                               12.f / 16.f, 4.f / 16.f,  14.f / 16.f, 6.f / 16.f,
                                               3.f / 16.f,  11.f / 16.f, 1.f / 16.f,  9.f / 16.f,
                                               15.f / 16.f, 7.f / 16.f,  13.f / 16.f, 5.f / 16.f };

namespace {

    void RenderTriTop(mt::buffer_t Buffer, mt::Triangle& Tri, mt::f32 InverseFar) {
        using namespace mt;

        auto [a, b, c]    = Tri.m_Vertices;
        texture_t Texture = GetTexture(Tri.m_TextureID);
        shader_t  Shade   = Tri.m_Shader;

        if (b.Pos.x < a.Pos.x) { std::swap(b, a); }

        f32       InvHeight = 1.f / (c.Pos.y - a.Pos.y);
        f32       InvWidth  = 1.f / (b.Pos.x - a.Pos.x);

        glm::vec2 lx        = { (c.Pos.x - a.Pos.x) * InvHeight, (c.Pos.x - b.Pos.x) * InvHeight };
        glm::vec2 lz        = { (b.Pos.z - a.Pos.z) * InvWidth, (c.Pos.z - a.Pos.z) * InvHeight };
        glm::vec2 lw        = { (b.Pos.w - a.Pos.w) * InvWidth, (c.Pos.w - a.Pos.w) * InvHeight };

        glm::vec2 lu        = { (b.UV.x - a.UV.x) * InvWidth, (c.UV.x - a.UV.x) * InvHeight };
        glm::vec2 lv        = { (b.UV.y - a.UV.y) * InvWidth, (c.UV.y - a.UV.y) * InvHeight };
        glm::vec2 ll        = { (b.Light - a.Light) * InvWidth, (c.Light - a.Light) * InvHeight };

        u32       Row       = static_cast<u32>(a.Pos.y * Buffer->GetWidth());

        for (u32 y = 0; y < u32(c.Pos.y - a.Pos.y); ++y) {
            u32 x0 = u32(a.Pos.x + y * lx.x);
            u32 x1 = u32(b.Pos.x + y * lx.y);

            f32 z0 = a.Pos.z + y * lz.y;
            f32 w0 = a.Pos.w + y * lw.y;
            f32 u0 = a.UV.x + y * lu.y;
            f32 v0 = a.UV.y + y * lv.y;
            f32 l0 = a.Light + y * ll.y;

            for (u32 x = x0; x <= x1; ++x) {
                f32 w = 1.f / w0;
                f32 z = glm::clamp(z0 * w * InverseFar, 0.f, 1.f);

                if (Buffer->GetDepth(Row + x) > z) {
                    f32 Light = glm::clamp(l0, GetSettingAmbientIntensity(), 1.f);

                    Shade(
                        Buffer, Texture, glm::vec4(x, y + static_cast<u32>(a.Pos.y), z, w),
                        glm::vec2(std::fabs(u0 * w), std::fabs(v0 * w)), Light, 1.f
                    );
                }

                else {
                    /* Buffer->SetPixel(
                        Row + x, BlendUint32(Buffer->GetPixel(Row + x), 0xFF0000FF, 0.5f)
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

    void RenderTriBottom(mt::buffer_t Buffer, mt::Triangle& Tri, mt::f32 InverseFar) {
        using namespace mt;

        auto& [a, b, c]   = Tri.m_Vertices;
        texture_t Texture = GetTexture(Tri.m_TextureID);
        shader_t  Shade   = Tri.m_Shader;

        if (c.Pos.x < b.Pos.x) { std::swap(c, b); }

        f32       InvHeight = 1.f / (b.Pos.y - a.Pos.y);
        f32       InvWidth  = 1.f / (c.Pos.x - b.Pos.x);

        glm::vec2 lx        = { (b.Pos.x - a.Pos.x) * InvHeight, (c.Pos.x - a.Pos.x) * InvHeight };
        glm::vec2 lz        = { (c.Pos.z - b.Pos.z) * InvWidth, (b.Pos.z - a.Pos.z) * InvHeight };
        glm::vec2 lw        = { (c.Pos.w - b.Pos.w) * InvWidth, (b.Pos.w - a.Pos.w) * InvHeight };

        glm::vec2 lu        = { (c.UV.x - b.UV.x) * InvWidth, (b.UV.x - a.UV.x) * InvHeight };
        glm::vec2 lv        = { (c.UV.y - b.UV.y) * InvWidth, (b.UV.y - a.UV.y) * InvHeight };
        glm::vec2 ll        = { (c.Light - b.Light) * InvWidth, (b.Light - a.Light) * InvHeight };

        u32       Row       = static_cast<u32>(a.Pos.y * Buffer->GetWidth());

        for (u32 y = 0; y <= u32(b.Pos.y - a.Pos.y); ++y) {
            u32 x0 = u32(a.Pos.x + y * lx.x);
            u32 x1 = u32(a.Pos.x + y * lx.y);

            f32 z0 = a.Pos.z + y * lz.y;
            f32 w0 = a.Pos.w + y * lw.y;
            f32 u0 = a.UV.x + y * lu.y;
            f32 v0 = a.UV.y + y * lv.y;
            f32 l0 = a.Light + y * ll.y;

            for (u32 x = x0; x <= x1; ++x) {
                f32 w = 1.f / w0;
                f32 z = glm::clamp(z0 * w * InverseFar, 0.f, 1.f);

                if (Buffer->GetDepth(Row + x) > z) {
                    f32 Light = glm::clamp(l0, GetSettingAmbientIntensity(), 1.f);

                    Shade(
                        Buffer, Texture, glm::vec4(x, y + static_cast<u32>(a.Pos.y), z, w),
                        glm::vec2(std::fabs(u0 * w), std::fabs(v0 * w)), Light, 1.f
                    );
                }

                else {
                    /* Buffer->SetPixel(
                        Row + x, BlendUint32(Buffer->GetPixel(Row + x), 0xFF0000FF, 0.5f)
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

namespace mt {

    void DrawWireframe(Context& Context, const Triangle& Tri, u8 r, u8 g, u8 b, u8 a) {
        glm::vec2 Ratio      = GetResolutionRatio();
        const auto [A, B, C] = Tri.m_Vertices;

        SDL_FPoint Points[4] = {
            {A.Pos.x * Ratio.x,  A.Pos.y * Ratio.y},
            { B.Pos.x * Ratio.x, B.Pos.y * Ratio.y},
            { C.Pos.x * Ratio.x, C.Pos.y * Ratio.y}
        };
        Points[3]          = Points[0];

        SDL_FRect Rects[3] = {
            {Points[0].x - 2.f,  Points[0].y - 2.f, 4.f, 4.f},
            { Points[1].x - 2.f, Points[1].y - 2.f, 4.f, 4.f},
            { Points[2].x - 2.f, Points[2].y - 2.f, 4.f, 4.f}
        };

        SDL_SetRenderDrawColor(Context.Renderer, r, g, b, a);
        SDL_RenderDrawLinesF(Context.Renderer, Points, 4);

        SDL_SetRenderDrawColor(Context.Renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRectsF(Context.Renderer, Rects, 3);
    }

    void RenderTriBary_NEW(buffer_t Buffer, Triangle& Tri, f32 InverseFar) {
        auto& [a, b, c] = Tri.m_Vertices;
        a.Pos.x *= Buffer->GetWidth(), a.Pos.y *= Buffer->GetHeight();
        b.Pos.x *= Buffer->GetWidth(), b.Pos.y *= Buffer->GetHeight();
        c.Pos.x *= Buffer->GetWidth(), c.Pos.y *= Buffer->GetHeight();

        auto [Min, Max] = Tri.GetBoundingBox();
        Min             = glm::clamp(Min, glm::vec2(0.f), Buffer->GetRect());
        Max             = glm::clamp(Max, glm::vec2(0.f), Buffer->GetRect());

        if (static_cast<u32>(Max.x - Min.x) * static_cast<u32>(Max.y - Min.y) == 0.f) {
            return; // zero area triangle
        }

        const auto& Shade   = Tri.m_Shader;
        texture_t   Texture = GetTexture(Tri.m_TextureID);

        const f32   InverseDoubleArea =
            1.f / DoubleTriangleArea(a.Pos.x, a.Pos.y, b.Pos.x, b.Pos.y, c.Pos.x, c.Pos.y);

        const glm::mat3 MatBary = {
            glm::cross(glm::vec3(a.Pos.x, b.Pos.x, c.Pos.x), glm::vec3(a.Pos.y, b.Pos.y, c.Pos.y)),
            glm::vec3(b.Pos.y - c.Pos.y, c.Pos.y - a.Pos.y, a.Pos.y - b.Pos.y),
            glm::vec3(c.Pos.x - b.Pos.x, a.Pos.x - c.Pos.x, b.Pos.x - a.Pos.x)
        };

        for (u32 y = static_cast<u32>(Min.y); y <= static_cast<u32>(Max.y); ++y) {
            u32 Row                   = y * Buffer->GetWidth();

            f32 x0                    = Min.x;
            /* for (; x0 < Max.x; ++x0) {
                const auto& BarysInside =
                    glm::greaterThanEqual(MatBary * glm::vec3(1.f, x0, y), glm::vec3(0.f));
                if (glm::all(BarysInside)) { break; }
            }*/

            f32 x1                    = Max.x;
            /* for (; x1 > Min.x; --x1) {
                const auto& BarysInside =
                    glm::greaterThanEqual(MatBary * glm::vec3(1.f, x1, y), glm::vec3(0.f));
                if (glm::all(BarysInside)) { break; }
            }*/

            glm::vec3       Barycoord = MatBary * glm::vec3(1.f, x0, y) * InverseDoubleArea;
            const glm::vec3 DBC =
                (MatBary * glm::vec3(1.f, x0 + 1, y) * InverseDoubleArea) - Barycoord;

            for (u32 x = static_cast<u32>(x0); x <= static_cast<u32>(x1); ++x, Barycoord += DBC) {
                f32 w =
                    1.f / (a.Pos.w * Barycoord.x + b.Pos.w * Barycoord.y + c.Pos.w * Barycoord.z);

                f32 z = glm::clamp(
                    (a.Pos.z * Barycoord.x + b.Pos.z * Barycoord.y + c.Pos.z * Barycoord.z) * w *
                        InverseFar,
                    0.f, 1.f
                );

                if (Buffer->GetDepth(Row + x) <= z) { continue; }

                f32 u =
                    std::fabs(a.UV.x * Barycoord.x + b.UV.x * Barycoord.y + c.UV.x * Barycoord.z) *
                    w;
                f32 v =
                    std::fabs(a.UV.y * Barycoord.x + b.UV.y * Barycoord.y + c.UV.y * Barycoord.z) *
                    w;

                f32 Light = glm::clamp(
                    a.Light * Barycoord.x + b.Light * Barycoord.y + c.Light * Barycoord.z,
                    GetSettingAmbientIntensity(), 1.f
                );

                Shade(Buffer, Texture, glm::vec4(x, y, z, w), glm::vec2(u, v), Light, Tri.m_Alpha);
            }
        }
    }

    void RenderTriScan(buffer_t Buffer, Triangle& Tri, f32 InverseFar) {
        auto& [a, b, c] = Tri.m_Vertices;
        a.Pos.x *= Buffer->GetWidth(), a.Pos.y *= Buffer->GetHeight();
        b.Pos.x *= Buffer->GetWidth(), b.Pos.y *= Buffer->GetHeight();
        c.Pos.x *= Buffer->GetWidth(), c.Pos.y *= Buffer->GetHeight();

        if (a.Pos.y > b.Pos.y) { std::swap(a, b); }
        if (b.Pos.y > c.Pos.y) { std::swap(b, c); }
        if (a.Pos.y > b.Pos.y) { std::swap(a, b); }

        if (static_cast<u32>(a.Pos.y) == static_cast<u32>(b.Pos.y)) {
            return RenderTriTop(Buffer, Tri, InverseFar);
        }

        if (static_cast<u32>(b.Pos.y) == static_cast<u32>(c.Pos.y)) {
            return RenderTriBottom(Buffer, Tri, InverseFar);
        }

        f32      LengthAtoD = (b.Pos.y - a.Pos.y) / (c.Pos.y - a.Pos.y);

        Vertex   d          = { .Pos    = a.Pos + (c.Pos - a.Pos) * LengthAtoD,
                                .Normal = glm::normalize(a.Normal + (c.Normal - a.Normal) * LengthAtoD),
                                .UV     = a.UV + (c.UV - a.UV) * LengthAtoD,
                                .Light  = glm::clamp(a.Light + (c.Light - a.Light) * LengthAtoD, 0.f, 1.f) };

        Triangle Top = Tri, Bottom = Tri;
        Top.m_Vertices    = { b, d, c };
        Bottom.m_Vertices = { a, d, b };

        RenderTriTop(Buffer, Top, InverseFar);
        RenderTriBottom(Buffer, Bottom, InverseFar);
    }

    void RenderTriBary(buffer_t Buffer, Triangle& Tri, f32 InverseFar) {
        auto& [a, b, c] = Tri.m_Vertices;
        a.Pos.x *= Buffer->GetWidth(), a.Pos.y *= Buffer->GetHeight();
        b.Pos.x *= Buffer->GetWidth(), b.Pos.y *= Buffer->GetHeight();
        c.Pos.x *= Buffer->GetWidth(), c.Pos.y *= Buffer->GetHeight();

        auto [Min, Max] = Tri.GetBoundingBox();
        Min             = glm::clamp(Min, glm::vec2(0.f), Buffer->GetRect() - 1.f);
        Max             = glm::clamp(Max, glm::vec2(0.f), Buffer->GetRect() - 1.f);

        if (static_cast<u32>(Max.x - Min.x) * static_cast<u32>(Max.y - Min.y) == 0.f) {
            return; // zero area triangle
        }

        const auto& Shade   = Tri.m_Shader;
        texture_t   Texture = GetTexture(Tri.m_TextureID);

        const f32   InverseDeterminant =
            1.f / Edge(a.Pos.x, a.Pos.y, b.Pos.x, b.Pos.y, c.Pos.x, c.Pos.y);

        for (u32 y = static_cast<u32>(Min.y); y <= static_cast<u32>(Max.y); ++y) {
            u32 Row     = y * Buffer->GetWidth();
            b8  Outside = true;

            for (u32 x = static_cast<u32>(Min.x); x <= static_cast<u32>(Max.x); ++x) {
                f32 B0 = Edge(b.Pos.x, b.Pos.y, c.Pos.x, c.Pos.y, x, y),
                    B1 = Edge(c.Pos.x, c.Pos.y, a.Pos.x, a.Pos.y, x, y),
                    B2 = Edge(a.Pos.x, a.Pos.y, b.Pos.x, b.Pos.y, x, y);

                if (B0 < 0.f && B1 < 0.f && B2 < 0.f) {
                    Outside = false;

                    B0 *= InverseDeterminant;
                    B1 *= InverseDeterminant;
                    B2 *= InverseDeterminant;

                    f32 w = 1.f / (a.Pos.w * B0 + b.Pos.w * B1 + c.Pos.w * B2);
                    f32 z = glm::clamp(
                        (a.Pos.z * B0 + b.Pos.z * B1 + c.Pos.z * B2) * w * InverseFar, 0.f, 1.f
                    );

                    if (Buffer->GetDepth(Row + x) <= z) { continue; }

                    f32 u     = std::fabs(a.UV.x * B0 + b.UV.x * B1 + c.UV.x * B2) * w;
                    f32 v     = std::fabs(a.UV.y * B0 + b.UV.y * B1 + c.UV.y * B2) * w;

                    f32 Light = glm::clamp(
                        a.Light * B0 + b.Light * B1 + c.Light * B2, GetSettingAmbientIntensity(),
                        1.f
                    );

                    Shade(
                        Buffer, Texture, glm::vec4(x, y, z, w), glm::vec2(u, v), Light, Tri.m_Alpha
                    );
                }

                else {
                    if (!Outside) { break; }
                }
            }
        }
    }
} // namespace mt

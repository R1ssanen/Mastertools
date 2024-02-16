#include "render.hpp"

#include <initializer_list>

#include "context.hpp"
#include "settings.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"
#include "buffer.hpp"

namespace
{

/*
void RenderTriTop(core::Context& t_Context, core::Vertex& a, core::Vertex& b, core::Vertex& c,
                  core::texture_t t_Texture, float t_InverseFar, const core::shader_t& t_Shader)
{
    if (b.m_Pos.x < a.m_Pos.x)          //     A     B
    {                                   //       C
        std::swap(a, b);
    }

    float ica = 1.f / (c.m_Pos.y - a.m_Pos.y), iba = 1.f / (b.m_Pos.x - a.m_Pos.x);

    float lx0 = (c.m_Pos.x - a.m_Pos.x) * ica, lx1 = (c.m_Pos.x - b.m_Pos.x) * ica, lz = (c.m_Pos.z - a.m_Pos.z) * ica,
          lg = (c.m_Light - a.m_Light) * ica, lu = (c.m_UV.x - a.m_UV.x) * ica, lv = (c.m_UV.y - a.m_UV.y) * ica,
          lw = (c.m_Pos.w - a.m_Pos.w) * ica;

    float lhz = (b.m_Pos.z - a.m_Pos.z) * iba, lhg = (b.m_Light - a.m_Light) * iba, lhu = (b.m_UV.x - a.m_UV.x) * iba,
          lhv = (b.m_UV.y - a.m_UV.y) * iba, lhw = (b.m_Pos.w - a.m_Pos.w) * iba;

    float x0 = a.m_Pos.x, x1 = b.m_Pos.x, z0 = a.m_Pos.z, w0 = a.m_Pos.w, u0 = a.m_UV.x, v0 = a.m_UV.y, g0 = a.m_Light;

    for (int y = static_cast<int>(a.m_Pos.y); y < static_cast<int>(c.m_Pos.y);
             y++, x0 += lx0, x1 += lx1, z0 += lz, w0 += lw, u0 += lu, v0 += lv, g0 += lg)
    {
        if (y < 0 || y >= t_Context.GetHeight()) {continue;}

        float z = z0, w = w0, u = u0, v = v0, g = g0;
        int row = y * t_Context.GetWidth();

        for (int x = static_cast<int>(x0); x <= static_cast<int>(x1);
                 x++, z += lhz, w += lhw, u += lhu, v += lhv, g += lhg)
        {
            if (x < 0 || x >= t_Context.GetWidth()) {continue;}

            float W = 1.f / w;
            float Z = glm::clamp(z * W * t_InverseFar, 0.f, 1.f);

            if (t_Context.DepthBuffer[row + x] <= Z)
            {
                continue;
            }

            float U = std::fabs(u) * W, V = std::fabs(v) * W;

            t_Shader(t_Context, t_Texture, glm::vec4(x, y, Z, W), glm::vec2(U, V), glm::clamp(g, 0.f, 1.f), nullptr);
        }
    }
}

void RenderTriBottom(core::Context& t_Context, core::Vertex& a, core::Vertex& b, core::Vertex& c,
                     core::texture_t t_Texture, float t_InverseFar, const core::shader_t& t_Shader)
{
    if (c.m_Pos.x < b.m_Pos.x)             //     A
    {                                      //   B      C
        std::swap(b, c);
    }

    float iba = 1.f / (b.m_Pos.y - a.m_Pos.y), icb = 1.f / (c.m_Pos.x - b.m_Pos.x);

    float lx0 = (b.m_Pos.x - a.m_Pos.x) * iba, lx1 = (c.m_Pos.x - a.m_Pos.x) * iba, lz = (b.m_Pos.z - a.m_Pos.z) * iba,
          lg = (b.m_Light - a.m_Light) * iba, lu = (b.m_UV.x - a.m_UV.x) * iba, lv = (b.m_UV.y - a.m_UV.y) * iba,
          lw = (b.m_Pos.w - a.m_Pos.w) * iba;

    float lhz = (c.m_Pos.z - b.m_Pos.z) * icb, lhg = (c.m_Light - b.m_Light) * icb, lhu = (c.m_UV.x - b.m_UV.x) * icb,
          lhv = (c.m_UV.y - b.m_UV.y) * icb, lhw = (c.m_Pos.w - b.m_Pos.w) * icb;

    float x0 = a.m_Pos.x, x1 = a.m_Pos.x, z0 = a.m_Pos.z, w0 = a.m_Pos.w, u0 = a.m_UV.x, v0 = a.m_UV.y, g0 = a.m_Light;

    for (int y = static_cast<int>(a.m_Pos.y); y < static_cast<int>(c.m_Pos.y);
             y++, x0 += lx0, x1 += lx1, z0 += lz, w0 += lw, u0 += lu, v0 += lv, g0 += lg)
    {
        if (y < 0 || y >= t_Context.GetHeight()) {continue;}

        float z = z0, w = w0, u = u0, v = v0, g = g0;
        int row = y * t_Context.GetWidth();

        for (int x = static_cast<int>(x0); x <= static_cast<int>(x1);
                 x++, z += lhz, w += lhw, u += lhu, v += lhv, g += lhg)
        {
            if (x < 0 || x >= t_Context.GetWidth()) {continue;}

            float W = 1.f / w;
            float Z = glm::clamp(z * W * t_InverseFar, 0.f, 1.f);

            if (t_Context.DepthBuffer[row + x] < Z)
            {
                continue;
            }

            float U = std::fabs(u) * W, V = std::fabs(v) * W;

            t_Shader(t_Context, t_Texture, glm::vec4(x, y, Z, W), glm::vec2(U, V), glm::clamp(g, 0.f, 1.f), nullptr);
        }
        
    }
}
*/
} // namespace

namespace core
{

void DrawWireframe(Context& t_Context, const triangle_t& t_Tri, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    glm::vec2 Ratio = GetResolutionRatio();

    SDL_FPoint Points[4] = {{t_Tri[0].m_Pos.x * Ratio.x, t_Tri[0].m_Pos.y * Ratio.y},
                            {t_Tri[1].m_Pos.x * Ratio.x, t_Tri[1].m_Pos.y * Ratio.y},
                            {t_Tri[2].m_Pos.x * Ratio.x, t_Tri[2].m_Pos.y * Ratio.y}};
    Points[3] = Points[0];

    SDL_FRect Rects[3] = {{Points[0].x - 2.f, Points[0].y - 2.f, 4.f, 4.f},
                            {Points[1].x - 2.f, Points[1].y - 2.f, 4.f, 4.f},
                            {Points[2].x - 2.f, Points[2].y - 2.f, 4.f, 4.f}};

    SDL_SetRenderDrawColor(t_Context.Renderer, r, g, b, a);
    SDL_RenderDrawLinesF(t_Context.Renderer, Points, 4);

    SDL_SetRenderDrawColor(t_Context.Renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRectsF(t_Context.Renderer, Rects, 3);
}

void RenderTriBary(buffer_t t_Buffer, DrawTri t_DrawTri, float t_InverseFar)
{
    auto [Min, Max] = GetBoundingBox(t_DrawTri.m_Tri);

    Min = glm::clamp(Min, glm::vec2(0.f), glm::vec2(t_Buffer->GetWidth() - 1, t_Buffer->GetHeight() - 1));
    Max = glm::clamp(Max, glm::vec2(0.f), glm::vec2(t_Buffer->GetWidth() - 1, t_Buffer->GetHeight() - 1));

    if (static_cast<unsigned>(Max.x - Min.x) * static_cast<unsigned>(Max.y - Min.y) == 0)
    {
        return; // zero area triangle
    }

    auto [a, b, c] = t_DrawTri.m_Tri;
    texture_t Texture = GetTexture(t_DrawTri.m_TextureID);

    const float InverseDoubleArea = 1.f / DoubleTriangleArea(a.m_Pos.x, a.m_Pos.y, b.m_Pos.x, b.m_Pos.y, c.m_Pos.x, c.m_Pos.y);
    const glm::vec3 Column1 = glm::cross(glm::vec3(a.m_Pos.x, b.m_Pos.x, c.m_Pos.x), glm::vec3(a.m_Pos.y, b.m_Pos.y, c.m_Pos.y)),
                    Column2 = glm::vec3(b.m_Pos.y - c.m_Pos.y, c.m_Pos.y - a.m_Pos.y, a.m_Pos.y - b.m_Pos.y),
                    Column3 = glm::vec3(c.m_Pos.x - b.m_Pos.x, a.m_Pos.x - c.m_Pos.x, b.m_Pos.x - a.m_Pos.x);
    
    const glm::mat3 MatBarycentric = {Column1, Column2, Column3};

    for (int y = Min.y; y <= Max.y; y++)
    {
        int Row = y * t_Buffer->GetWidth();
        bool Outside = true;

        for (int x = Min.x; x <= Max.x; x++)
        {
            glm::vec3 Barycoord = MatBarycentric * glm::vec3(1, x, y);
            if (Barycoord.x >= 0.f && Barycoord.y >= 0.f && Barycoord.z >= 0.f)
            {
                Outside = false;
                Barycoord *= InverseDoubleArea;

                float w = 1.f / (a.m_Pos.w * Barycoord.x + b.m_Pos.w * Barycoord.y + c.m_Pos.w * Barycoord.z);
                float z = glm::clamp((a.m_Pos.z * Barycoord.x + b.m_Pos.z * Barycoord.y + c.m_Pos.z * Barycoord.z) *
                                        w * t_InverseFar, 0.f, 1.f);

                if (t_Buffer->GetDepth(Row + x) <= z)
                {
                    continue;
                }

                float u = std::fabs(Barycoord.x * a.m_UV.x + Barycoord.y * b.m_UV.x + Barycoord.z * c.m_UV.x) * w;
                float v = std::fabs(Barycoord.x * a.m_UV.y + Barycoord.y * b.m_UV.y + Barycoord.z * c.m_UV.y) * w;

                float Light = glm::clamp(Barycoord.x * a.m_Light + Barycoord.y * b.m_Light + Barycoord.z * c.m_Light,
                                         GetSettingAmbientIntensity(), 1.f);

                uint8_t R = glm::dot(Barycoord, glm::vec3(a.m_LightColor.r, b.m_LightColor.r, c.m_LightColor.r)) * 255,
                        G = glm::dot(Barycoord, glm::vec3(a.m_LightColor.g, b.m_LightColor.g, c.m_LightColor.g)) * 255,
                        B = glm::dot(Barycoord, glm::vec3(a.m_LightColor.b, b.m_LightColor.b, c.m_LightColor.b)) * 255;                
                uint32_t LightColor = ToUint32(glm::clamp(glm::vec4(R, G, B, 255.f), glm::vec4(0.f), glm::vec4(255.f)));

                t_DrawTri.m_Shader(t_Buffer, Texture, glm::vec4(x, y, z, w), glm::vec2(u, v), Light, static_cast<void*>(&LightColor));
            }

            else
            {
                if (!Outside)
                {
                    break;
                }
            }
        }
    }
}

/*
void RenderTriScan(Context& t_Context, triangle_t& t_Tri, texture_t t_Texture, float t_InverseFar,
                   const shader_t& t_Shader)
{
    std::sort(t_Tri.begin(), t_Tri.end(), [] (const Vertex& A, const Vertex& B) { return A.m_Pos.y < B.m_Pos.y; });

    auto [a, b, c] = t_Tri;

    if (static_cast<int>(a.m_Pos.y) >= t_Context.GetHeight()) [[likely]] {return;}

    else if (static_cast<int>(a.m_Pos.y) == static_cast<int>(b.m_Pos.y))
    {
        RenderTriTop(t_Context, a, b, c, t_Texture, t_InverseFar, t_Shader);
    }

    else if (static_cast<int>(b.m_Pos.y) == static_cast<int>(c.m_Pos.y))
    {
        RenderTriBottom(t_Context, a, b, c, t_Texture, t_InverseFar, t_Shader);
    }

    else [[likely]]
    {
        float LengthAtoD = (b.m_Pos.y - a.m_Pos.y) / (c.m_Pos.y - a.m_Pos.y);

        Vertex d = Vertex(a.m_Pos + (c.m_Pos - a.m_Pos) * LengthAtoD,
                          glm::normalize(a.m_Normal + (c.m_Normal - a.m_Normal) * LengthAtoD),
                          a.m_UV + (c.m_UV - a.m_UV) * LengthAtoD,
                          glm::clamp(a.m_Light + (c.m_Light - a.m_Light) * LengthAtoD, 0.f, 1.f));

        RenderTriTop(t_Context, b, d, c, t_Texture, t_InverseFar, t_Shader);
        RenderTriBottom(t_Context, a, d, b, t_Texture, t_InverseFar, t_Shader);
    }
}
*/
} // namespace core

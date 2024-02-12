#pragma once

#include "settings.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace core
{

class Vertex
{
  public:
    Vertex(const glm::vec4& t_Pos, const glm::vec4& t_Normal, const glm::vec2& t_UV,
           float t_Light = GetSettingAmbientIntensity())
        : m_Pos{t_Pos}, m_Normal{t_Normal}, m_UV{t_UV}, m_Light{t_Light}
    {
    }

    glm::vec4 m_Pos{0.f, 0.f, 0.f, 1.f}, m_Normal{0.f};
    glm::vec2 m_UV{0.f};
    float m_Light;
};

using vertex_vector_t = std::vector<Vertex>;
using index_vector_t = std::vector<unsigned>;

using triangle_t = std::array<Vertex, 3>;
using triangle_vector_t = std::vector<triangle_t>;

inline std::pair<glm::vec2, glm::vec2> GetBoundingBox(const triangle_t& t_Tri) {
    auto [MinX, MaxX] = std::minmax({t_Tri[0].m_Pos.x, t_Tri[1].m_Pos.x, t_Tri[2].m_Pos.x});
    auto [MinY, MaxY] = std::minmax({t_Tri[0].m_Pos.y, t_Tri[1].m_Pos.y, t_Tri[2].m_Pos.y});

    return std::pair<glm::vec2, glm::vec2>{glm::vec2(MinX, MinY), glm::vec2(MaxX, MaxY)};
}

struct DrawTri
{
    triangle_t m_Tri;
    uint32_t m_TextureID;

    static bool FarToClose(const DrawTri& t_First, const DrawTri& t_Second)
    {
        return (t_First.m_Tri[0].m_Pos.z + t_First.m_Tri[1].m_Pos.z + t_First.m_Tri[2].m_Pos.z) * glm::third<float>() >
               (t_Second.m_Tri[0].m_Pos.z + t_Second.m_Tri[1].m_Pos.z + t_Second.m_Tri[2].m_Pos.z) *
                   glm::third<float>();
    }

    static bool CloseToFar(const DrawTri& t_First, const DrawTri& t_Second) { return !FarToClose(t_First, t_Second); }
};

} // namespace core

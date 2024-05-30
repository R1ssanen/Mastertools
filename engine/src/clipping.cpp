#include "clipping.hpp"

#include "mtpch.hpp"
#include "triangle.hpp"
#include "texture.hpp"

namespace core
{

f32 Plane::SignedDistance(const glm::vec3& Point) const { return glm::dot(m.Normal, Point - m.Point); }

Plane Plane::New(const glm::vec3& Normal, const glm::vec3& Point) {
    return Plane(
        _M{
            .Point = Point,
            .Normal = Normal
        }
    );
}

Vertex IntersectLine(const Plane& Plane, const Vertex& A, const Vertex& B)
{
    const f32 T = glm::clamp(
            glm::dot(Plane.GetPoint() - glm::vec3(A.m_Pos), Plane.GetNormal()) /
            glm::dot(glm::vec3(B.m_Pos - A.m_Pos), Plane.GetNormal()), 0.f, 1.f);

    return Vertex{
        .m_Pos = A.m_Pos + (B.m_Pos - A.m_Pos) * T,
        .m_Normal = glm::normalize(A.m_Normal + (B.m_Normal - A.m_Normal) * T),
        .m_LightColor = A.m_LightColor + (B.m_LightColor - A.m_LightColor) * T,
        .m_UV = A.m_UV + (B.m_UV - A.m_UV) * T,
        .m_Light = std::min(A.m_Light + (B.m_Light - A.m_Light) * T, 1.f)
    };
}

void ClipTriangle(Triangle& Tri, const Plane& Plane, triangle_vector_t& o_ClipTriangles)
{
    auto [a, b, c] = Tri.m_Vertices;
    Vertex Temp;

    switch (((Plane.SignedDistance(c.m_Pos) > 0) << 2)
         + ((Plane.SignedDistance(b.m_Pos) > 0) << 1)
         + (Plane.SignedDistance(a.m_Pos) > 0))
    {
        case 0: return;

        case 7:
            o_ClipTriangles.push_back(std::move(Tri)); return;

        case 1:
            Tri.m_Vertices = {a, IntersectLine(Plane, a, b), IntersectLine(Plane, a, c)};
            o_ClipTriangles.push_back(std::move(Tri)); return;
        
        case 2:
            Tri.m_Vertices = {IntersectLine(Plane, b, a), b, IntersectLine(Plane, b, c)};
            o_ClipTriangles.push_back(std::move(Tri)); return;
        
        case 4:
            Tri.m_Vertices = {IntersectLine(Plane, c, a), IntersectLine(Plane, c, b), c};
            o_ClipTriangles.push_back(std::move(Tri)); return;

        case 3:
            Temp = IntersectLine(Plane, a, c);
            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{a, b, Temp},
                Tri.m_Shader, Tri.m_TextureID, 1.f
            );

            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{Temp, b, IntersectLine(Plane, b, c)},
                Tri.m_Shader, Tri.m_TextureID, 1.f
            ); return;

        case 5:
            Temp = IntersectLine(Plane, a, b);
            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{a, Temp, c},
                Tri.m_Shader, Tri.m_TextureID, 1.f
            );
            
            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{Temp, IntersectLine(Plane, c, b), c},
                Tri.m_Shader, Tri.m_TextureID, 1.f
            ); return;

        default: // 6
            Temp = IntersectLine(Plane, b, a);
            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{Temp, b, c},
                Tri.m_Shader, Tri.m_TextureID, 1.f
            );

            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{IntersectLine(Plane, c, a), Temp, c},
                Tri.m_Shader, Tri.m_TextureID, 1.f
            ); return;
    }
}

void FrustumClipTriangle(const Triangle& Tri, const Frustum& Frustum, triangle_vector_t& o_ClipTriangles)
{
    triangle_vector_t ClipTriangles = {Tri};

    for (const Plane& Plane : Frustum)
    {
        triangle_vector_t Temp;

        for (Triangle& Tri : ClipTriangles)
        {
            ClipTriangle(Tri, Plane, Temp);
        }

        ClipTriangles = std::move(Temp);
    }

    o_ClipTriangles.insert(o_ClipTriangles.end(), ClipTriangles.begin(), ClipTriangles.end());
}

} // namespace core

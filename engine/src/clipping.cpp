#include "clipping.hpp"

#include "srpch.hpp"

namespace core
{

float Plane::SignedDistance(const glm::vec3& t_Point) const { return glm::dot(m.Normal, t_Point - m.Point); }

Plane Plane::New(const glm::vec3& t_Normal, const glm::vec3& t_Point) {
    return Plane(
        _M{
            .Point = t_Point,
            .Normal = t_Normal
        }
    );
}

Vertex IntersectLine(const Plane& t_Plane, const Vertex& A, const Vertex& B)
{
    const float T = glm::clamp(glm::dot(t_Plane.GetPoint() - glm::vec3(A.m_Pos), t_Plane.GetNormal()) /
                                   glm::dot(glm::vec3(B.m_Pos - A.m_Pos), t_Plane.GetNormal()),
                               0.f, 1.f);

    glm::vec4 Pos = A.m_Pos + (B.m_Pos - A.m_Pos) * T;
    glm::vec4 Normal = glm::normalize(A.m_Normal + (B.m_Normal - A.m_Normal) * T);
    glm::vec2 UV = A.m_UV + (B.m_UV - A.m_UV) * T;
    float Light = std::min(A.m_Light + (B.m_Light - A.m_Light) * T, 1.f);

    return Vertex(Pos, Normal, UV, Light);
}

glm::vec3 IntersectLine(const Plane& t_Plane, const glm::vec3& A, const glm::vec3& B)
{
    const float T = glm::clamp(glm::dot(t_Plane.GetPoint() - glm::vec3(A), t_Plane.GetNormal()) /
                                   glm::dot(glm::vec3(B - A), t_Plane.GetNormal()),
                               0.f, 1.f);

    return A + (B - A) * T;
}

triangle_vector_t ClipTriangle(const triangle_t& t_Tri, const Plane& t_Plane)
{
    const auto [a, b, c] = t_Tri;

    switch (((t_Plane.SignedDistance(c.m_Pos) >= 0) << 2) + ((t_Plane.SignedDistance(b.m_Pos) >= 0) << 1) +
            (t_Plane.SignedDistance(a.m_Pos) >= 0))
    {
    case 0:
        return triangle_vector_t{};
    case 7:
        return triangle_vector_t{triangle_t{a, b, c}};

    case 1:
        return triangle_vector_t{triangle_t{a, IntersectLine(t_Plane, a, b), IntersectLine(t_Plane, a, c)}};
    case 2:
        return triangle_vector_t{triangle_t{IntersectLine(t_Plane, b, a), b, IntersectLine(t_Plane, b, c)}};
    case 4:
        return triangle_vector_t{triangle_t{IntersectLine(t_Plane, c, a), IntersectLine(t_Plane, c, b), c}};

    case 3:
        return triangle_vector_t{triangle_t{a, b, IntersectLine(t_Plane, a, c)},
                                 triangle_t{IntersectLine(t_Plane, a, c), b, IntersectLine(t_Plane, b, c)}};
    case 5:
        return triangle_vector_t{triangle_t{a, IntersectLine(t_Plane, a, b), c},
                                 triangle_t{IntersectLine(t_Plane, a, b), IntersectLine(t_Plane, c, b), c}};
    default:
        return triangle_vector_t{triangle_t{IntersectLine(t_Plane, b, a), b, c},
                                 triangle_t{IntersectLine(t_Plane, c, a), IntersectLine(t_Plane, b, a), c}};
    }
}

triangle_vector_t FrustumClipTriangle(const triangle_t& t_Tri, const Frustum& t_Frustum)
{
    triangle_vector_t ClipTris{t_Tri};

    for (const Plane& Plane : t_Frustum)
    {
        triangle_vector_t Temp;

        for (const triangle_t& Tri : ClipTris)
        {
            const auto NewTriangles{ClipTriangle(Tri, Plane)};
            Temp.insert(Temp.end(), NewTriangles.begin(), NewTriangles.end());
        }

        ClipTris = Temp;
    }

    return ClipTris;
}

} // namespace core

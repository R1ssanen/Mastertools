#include "clipping.hpp"

#include "srpch.hpp"

namespace core {

float Plane::SignedDistance(const glm::vec3& t_Pos) const {
  return glm::dot(m_Normal, t_Pos - m_Point);
}

Vertex IntersectVertexLine(const Plane& t_Plane,
                           const Vertex& A,
                           const Vertex& B) {
  const float T = glm::clamp(
      glm::dot(t_Plane.GetPoint() - glm::vec3(A.m_Pos), t_Plane.GetNormal()) /
          glm::dot(glm::vec3(B.m_Pos - A.m_Pos), t_Plane.GetNormal()),
      0.f, 1.f);

  glm::vec4 Pos = A.m_Pos + (B.m_Pos - A.m_Pos) * T,
            Normal = A.m_Normal + (B.m_Normal - A.m_Normal) * T;
  glm::vec2 UV = A.m_UV + (B.m_UV - A.m_UV) * T;
  float Light = std::min(A.m_Light + (B.m_Light - A.m_Light) * T, 1.f);

  return Vertex(Pos, glm::fastNormalize(Normal), UV, Light);
}

glm::vec3 IntersectLine(const Plane& t_Plane,
                        const glm::vec3& A,
                        const glm::vec3& B) {
  const float T = glm::clamp(
      glm::dot(t_Plane.GetPoint() - glm::vec3(A), t_Plane.GetNormal()) /
          glm::dot(glm::vec3(B - A), t_Plane.GetNormal()),
      0.f, 1.f);

  return A + (B - A) * T;
}

std::vector<Tri> ClipTriangle(const Tri& t_Tri, const Plane& t_Plane) {
  const auto [a, b, c] = t_Tri;

  switch (((t_Plane.SignedDistance(c.m_Pos) >= 0) << 2) +
          ((t_Plane.SignedDistance(b.m_Pos) >= 0) << 1) +
          (t_Plane.SignedDistance(a.m_Pos) >= 0)) {
    case 0:
      return std::vector<Tri>{};
    case 7:
      return std::vector<Tri>{Tri{a, b, c}};

    case 1:
      return std::vector<Tri>{Tri{a, IntersectVertexLine(t_Plane, a, b),
                                  IntersectVertexLine(t_Plane, a, c)}};
    case 2:
      return std::vector<Tri>{Tri{IntersectVertexLine(t_Plane, b, a), b,
                                  IntersectVertexLine(t_Plane, b, c)}};
    case 4:
      return std::vector<Tri>{Tri{IntersectVertexLine(t_Plane, c, a),
                                  IntersectVertexLine(t_Plane, c, b), c}};

    case 3:
      return std::vector<Tri>{Tri{a, b, IntersectVertexLine(t_Plane, a, c)},
                              Tri{IntersectVertexLine(t_Plane, a, c), b,
                                  IntersectVertexLine(t_Plane, b, c)}};
    case 5:
      return std::vector<Tri>{Tri{a, IntersectVertexLine(t_Plane, a, b), c},
                              Tri{IntersectVertexLine(t_Plane, a, b),
                                  IntersectVertexLine(t_Plane, c, b), c}};
    default:
      return std::vector<Tri>{Tri{IntersectVertexLine(t_Plane, b, a), b, c},
                              Tri{IntersectVertexLine(t_Plane, c, a),
                                  IntersectVertexLine(t_Plane, b, a), c}};
  }
}

std::vector<Tri> FrustumClipTriangle(const Tri& t_Tri,
                                     const Frustum& t_Frustum) {
  std::vector<Tri> ClipTris{t_Tri};

  for (const Plane& Plane : t_Frustum) {
    std::vector<Tri> Temp;

    for (const Tri& Tri : ClipTris) {
      const auto NewTriangles{ClipTriangle(Tri, Plane)};
      Temp.insert(Temp.end(), NewTriangles.begin(), NewTriangles.end());
    }

    ClipTris = Temp;
  }

  return ClipTris;
}

}  // namespace core

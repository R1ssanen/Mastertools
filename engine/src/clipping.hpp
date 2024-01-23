#pragma once

#include "srpch.hpp"
#include "vertex.hpp"

namespace core {

class Plane {
 public:
  ~Plane() = default;
  Plane() = default;

  Plane(const glm::vec3& t_Point, const glm::vec3& t_Normal)
      : m_Point{t_Point}, m_Normal{t_Normal} {}

  float SignedDistance(const glm::vec3& t_Pos) const;

  const glm::vec3& GetPoint() const noexcept { return m_Point; }
  const glm::vec3& GetNormal() const noexcept { return m_Normal; }

 private:
  glm::vec3 m_Point, m_Normal;
};

using Frustum = std::vector<Plane>;

Vertex IntersectVertexLine(const Plane& t_Plane,
                           const Vertex& A,
                           const Vertex& B);

glm::vec3 IntersectLine(const Plane& t_Plane,
                        const glm::vec3& A,
                        const glm::vec3& B);

std::vector<Tri> ClipTriangle(const Tri& t_Tri, const Plane& t_Plane);

std::vector<Tri> FrustumClipTriangle(const Tri& t_Tri,
                                     const Frustum& t_Frustum);

}  // namespace core
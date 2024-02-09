#pragma once

#include "srpch.hpp"
#include "vertex.hpp"

namespace core
{

class Plane
{
  public:
    Plane(const glm::vec3& t_Point, const glm::vec3& t_Normal) : m_Point{t_Point}, m_Normal{t_Normal} {}

    float SignedDistance(const glm::vec3& t_Point) const { return glm::dot(m_Normal, t_Point - m_Point); }

    const glm::vec3& GetPoint() const { return m_Point; }
    const glm::vec3& GetNormal() const { return m_Normal; }

  private:
    glm::vec3 m_Point, m_Normal;
};

using Frustum = std::vector<Plane>;

Vertex IntersectLine(const Plane& t_Plane, const Vertex& A, const Vertex& B);

glm::vec3 IntersectLine(const Plane& t_Plane, const glm::vec3& A, const glm::vec3& B);

triangle_vector_t ClipTriangle(const triangle_t& t_Tri, const Plane& t_Plane);

triangle_vector_t FrustumClipTriangle(const triangle_t& t_Tri, const Frustum& t_Frustum);

} // namespace core
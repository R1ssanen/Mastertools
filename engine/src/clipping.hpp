#pragma once

#include "srpch.hpp"
#include "vertex.hpp"

namespace core
{

class Plane
{
  public:
    float SignedDistance(const glm::vec3& t_Point) const;

    const glm::vec3& GetPoint() const { return m.Point; }
    const glm::vec3& GetNormal() const { return m.Normal; }

    static Plane New(const glm::vec3& t_Normal, const glm::vec3& t_Point = glm::vec3(0.f));

  private:
    struct _M {
        glm::vec3 Point;
        glm::vec3 Normal;
    } m;

    Plane(_M&& t_Data) : m{std::move(t_Data)} {}
};

using Frustum = std::vector<Plane>;

Vertex IntersectLine(const Plane& t_Plane, const Vertex& A, const Vertex& B);

glm::vec3 IntersectLine(const Plane& t_Plane, const glm::vec3& A, const glm::vec3& B);

triangle_vector_t ClipTriangle(const triangle_t& t_Tri, const Plane& t_Plane);

triangle_vector_t FrustumClipTriangle(const triangle_t& t_Tri, const Frustum& t_Frustum);

} // namespace core
#pragma once

#include "mtpch.hpp"
#include "vertex.hpp"
#include "triangle.hpp"

namespace core
{

class Plane
{
  public:
    f32 SignedDistance(const glm::vec3& Point) const;

    const glm::vec3& GetPoint() const { return m.Point; }
    const glm::vec3& GetNormal() const { return m.Normal; }

    static Plane New(const glm::vec3& Normal, const glm::vec3& Point = glm::vec3(0.f));

  private:
    struct _M {
        glm::vec3 Point;
        glm::vec3 Normal;
    } m;

    Plane(_M&& Data) : m{std::move(Data)} {}
};

using Frustum = std::vector<Plane>;

Vertex IntersectLine(const Plane& Plane, const Vertex& A, const Vertex& B);

void ClipTriangle(Triangle& Tri, const Plane& Plane, triangle_vector_t& o_ClipTriangles);

void FrustumClipTriangle(const Triangle& Tri, const Frustum& Frustum, triangle_vector_t& o_ClipTriangles);

} // namespace core
#pragma once

#include "../core/vertex.hpp"
#include "../mtpch.hpp"
#include "aabb.hpp"
#include "plane.hpp"
#include "ray.hpp"

namespace mt::math {

    enum class VolumeIntersectionFlag { IN, OUT, INTERSECTS };

    u32    GetDebugCullColor(VolumeIntersectionFlag Flag);

    Vertex IntersectLinePlane(const math::Plane& Plane, const Vertex& A, const Vertex& B);

    VolumeIntersectionFlag   IntersectPlaneAABB(const math::AABB& AABB, const math::Plane& Plane);

    std::optional<glm::vec3> IntersectRayTriangle(
        const math::Ray& Ray, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C
    );

} // namespace mt::math

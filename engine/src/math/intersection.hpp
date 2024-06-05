#pragma once

#include "../core/vertex.hpp"
#include "../mtpch.hpp"
#include "aabb.hpp"
#include "plane.hpp"

namespace mt::math {

    enum class VolumeIntersectionFlag { IN, OUT, INTERSECTS };

    Vertex                 IntersectLinePlane(const Plane& Plane, const Vertex& A, const Vertex& B);

    VolumeIntersectionFlag IntersectPlaneAABB(const math::AABB& AABB, const math::Plane& Plane);

    u32                    GetDebugCullColor(VolumeIntersectionFlag Flag);

} // namespace mt::math

#include "plane.hpp"

#include "../mtpch.hpp"

namespace mt::math {

    f32 Plane::SignedDistance(const glm::vec3& Point) const {
        return glm::dot(m.Normal, Point - m.Point);
    }

    Plane::Plane(const glm::vec3& Normal, const glm::vec3& Point)
        : m(_M{ .Point = Point, .Normal = Normal }) { }

} // namespace mt::math

#include "ray.hpp"

#include "../mtpch.hpp"

namespace mt::math {

    glm::vec3 Ray::Trace(f32 Scalar) const { return m.Origin + m.Direction * Scalar; }

    Ray::Ray(const glm::vec3& Origin, const glm::vec3& Direction)
        : m(_M{ .Origin = Origin, .Direction = Direction }) { }

} // namespace mt::math

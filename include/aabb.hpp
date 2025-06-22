#ifndef MT_AABB_HPP_
#define MT_AABB_HPP_

#include <algorithm>
#include <glm/vec3.hpp>

#include "mtdefs.hpp"

namespace mt {

    class AABB {
      public:

        AABB(const glm::vec3& center, const glm::vec3& sides)
            : m_center(center), m_min(center - sides * 0.5f), m_max(center + sides * 0.5f) { }

        // NOTE: for correct behavior, ray origin must
        //       be within the AABB bounds
        bool from_inside_intersect_ray(
            const glm::vec3& origin, const glm::vec3& direction, glm::vec3& intersection
        ) const;

        const glm::vec3& center() const noexcept { return m_center; }

      private:

        glm::vec3 m_center;
        glm::vec3 m_min;
        glm::vec3 m_max;
    };

} // namespace mt

namespace mt {

    inline bool AABB::from_inside_intersect_ray(
        const glm::vec3& origin, const glm::vec3& direction, glm::vec3& intersection
    ) const {

        glm::bvec3 greater_zero_mask = glm::greaterThan(direction, glm::vec3(0.f));
        glm::vec3  bounds            = glm::mix(m_min, m_max, greater_zero_mask);
        bounds                       = (bounds - origin) / direction;

        intersection = origin + std::min({ bounds.x, bounds.y, bounds.z }) * direction;
        return true;
    }

} // namespace mt
#endif

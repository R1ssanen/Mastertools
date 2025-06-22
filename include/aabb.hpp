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

        // t = (p - o) / d
        glm::vec3 inv_direction = 1.f / direction;

        glm::vec3 t_low         = (m_min - origin) * inv_direction;
        glm::vec3 t_high        = (m_max - origin) * inv_direction;

        auto [tx_near, tx_far]  = std::minmax(t_low.x, t_high.x);
        auto [ty_near, ty_far]  = std::minmax(t_low.y, t_high.y);
        auto [tz_near, tz_far]  = std::minmax(t_low.z, t_high.z);

        f32 t_near              = std::max({ tx_near, ty_near, tz_near });
        // if (t_near <= 0.f) return false;

        f32 t_far               = std::min({ tx_far, ty_far, tz_far });
        // if (t_far <= 0.f) return false;

        if (t_far <= t_near) return false;

        intersection = origin + t_far * direction;
        return true;
    }

} // namespace mt
#endif

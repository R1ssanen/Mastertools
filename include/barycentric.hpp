#ifndef MT_BARYCENTRIC_HPP_
#define MT_BARYCENTRIC_HPP_

#include <glm/vec3.hpp>
#include <tuple>

#include "mtdefs.hpp"

namespace mt {

    class Barycentric {
      public:

        Barycentric(
            const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, bool skip_if_backface = true
        ) {

            f32 det = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);

            if (det == 0) {
                is_valid = false;
                return;

            } else if (det > 0) {
                is_backface = true;
                if (skip_if_backface) return;
            }

            m_inv_area_2 = 1.f / det;
            m_mat        = glm::mat3(
                { b.x * c.y - c.x * b.y, b.y - c.y, c.x - b.x },
                { c.x * a.y - a.x * c.y, c.y - a.y, a.x - c.x },
                { a.x * b.y - b.x * a.y, a.y - b.y, b.x - a.x }
            );

            m_dx = glm::vec3(b.y - c.y, c.y - a.y, a.y - b.y) * m_inv_area_2;
            m_dy = glm::vec3(c.x - b.x, a.x - c.x, b.x - a.x) * m_inv_area_2;
        }

        glm::vec3 GetDeltaX(void) const noexcept { return m_dx; }

        glm::vec3 GetDeltaY(f32 slope) const noexcept { return m_dy + slope * m_dx; }

        glm::vec3 GetCoord(f32 x, f32 y) const noexcept {
            return m_inv_area_2 * glm::vec3(1.f, x, y) * m_mat;
        }

        bool is_backface = false;
        bool is_valid    = true;

      private:

        glm::mat3 m_mat;
        glm::vec3 m_dx;
        glm::vec3 m_dy;
        f32       m_inv_area_2;
    };

} // namespace mt

#endif

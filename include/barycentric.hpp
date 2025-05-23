#ifndef MT_BARYCENTRIC_HPP_
#define MT_BARYCENTRIC_HPP_

#include <glm/mat3x3.hpp>
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

            m_mat = glm::mat3(
                { b.x * c.y - c.x * b.y, b.y - c.y, c.x - b.x },
                { c.x * a.y - a.x * c.y, c.y - a.y, a.x - c.x },
                { a.x * b.y - b.x * a.y, a.y - b.y, b.x - a.x }
            );

            m_inv_area_2 = 1.f / det;

            m_dx0        = (b.y - c.y) * m_inv_area_2;
            m_dx1        = (c.y - a.y) * m_inv_area_2;
            m_dx2        = (a.y - b.y) * m_inv_area_2;

            m_dy0        = (c.x - b.x) * m_inv_area_2;
            m_dy1        = (a.x - c.x) * m_inv_area_2;
            m_dy2        = (b.x - a.x) * m_inv_area_2;
        }

        auto GetDeltaX(void) const { return std::make_tuple(m_dx0, m_dx1, m_dx2); }

        auto GetDeltaY(f32 slope) const {
            return std::make_tuple(
                slope * m_dx0 + m_dy0, slope * m_dx1 + m_dy1, slope * m_dx2 + m_dy2
            );
        }

        auto GetCoord(f32 x, f32 y) const {
            auto bc = m_inv_area_2 * glm::vec3(1.f, x, y) * m_mat;
            return std::make_tuple(bc.x, bc.y, bc.z);
        }

        bool is_backface = false;
        bool is_valid    = true;

      private:

        glm::mat3 m_mat;
        f32       m_dx0, m_dx1, m_dx2;
        f32       m_dy0, m_dy1, m_dy2;
        f32       m_inv_area_2;
    };

} // namespace mt

#endif

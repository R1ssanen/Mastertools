#ifndef MT_BARYCENTRIC_HPP_
#define MT_BARYCENTRIC_HPP_

#include <glm/mat3x3.hpp>

#include "mtdefs.hpp"

namespace mt {

#if 1
    class Barycentric {
      public:

        Barycentric(
            const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, bool skip_if_backface = true
        )
            : m_is_backface(false) {

            f32 determinant = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);

            if (determinant <= 0) {
                m_is_backface = true;
                if (skip_if_backface) return;
            }

            m_inv_area_2 = 1.f / determinant;
            m_mat        = glm::mat3(
                glm::vec3(b.x * c.y - c.x * b.y, b.y - c.y, c.x - b.x),
                glm::vec3(c.x * a.y - a.x * c.y, c.y - a.y, a.x - c.x),
                glm::vec3(a.x * b.y - b.x * a.y, a.y - b.y, b.x - a.x)
            );

            m_start = Get(a.x, a.y);
            m_dx    = m_start - Get(a.x + 1.f, a.y);
            m_dy    = m_start - Get(a.x, a.y + 1.f);
        }

        glm::vec3 Get(f32 x, f32 y) const { return m_inv_area_2 * glm::vec3(1.f, x, y) * m_mat; }

        bool      IsBackface(void) const { return m_is_backface; }

      private:

        glm::mat3 m_mat;
        glm::vec3 m_dx, m_dy, m_start;
        f32       m_inv_area_2;
        bool      m_is_backface;
    };
#else
    class Barycentric {
      public:

        Barycentric(
            const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, bool skip_if_backface = true
        )
            : m_is_backface(false) {

            f32 determinant = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);

            if (determinant <= 0) {
                m_is_backface = true;
                if (skip_if_backface) return;
            }

            A01 = a.pos.y - b.pos.y;
            A12 = b.pos.y - c.pos.y;
            A20 = c.pos.y - a.pos.y;

            B01 = b.pos.x - a.pos.x;
            B12 = c.pos.x - b.pos.x;
            B20 = a.pos.x - c.pos.x;
        }

        // glm::vec3 Get(f32 x, f32 y) const { return m_inv_area_2 * glm::vec3(1.f, x, y) * m_mat; }

        bool IsBackface(void) const { return m_is_backface; }

      private:

        f32  A01, B01;
        f32  A12, B12;
        f32  A20, B20;
        bool m_is_backface;
    };

#endif

} // namespace mt

#endif

#ifndef MT_PLANE_HPP_
#define MT_PLANE_HPP_

#include <glm/ext/vector_float3.hpp>

#include "mtdefs.hpp"

namespace mt {

    class Plane {
      public:

        Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

        Plane(const glm::vec3& pos, const glm::vec3& normal);

        bool IntersectsLine(const glm::vec3& a, const glm::vec3& b, glm::vec3& intersection) const;

      private:

        glm::vec3 m_normal;
        f32       m_d;
    };

} // namespace mt

#endif

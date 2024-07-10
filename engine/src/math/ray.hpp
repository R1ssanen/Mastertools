#pragma once

#include "../mtpch.hpp"

namespace mt::math {

    class Ray {
      public:

        glm::vec3        Trace(f32 Scalar) const;

        const glm::vec3& GetOrigin() const { return m.Origin; }

        const glm::vec3& GetDirection() const { return m.Direction; }

        Ray(const glm::vec3& Origin, const glm::vec3& Direction);

      private:

        struct _M {
            glm::vec3 Origin;
            glm::vec3 Direction;
        } m;
    };

} // namespace mt::math

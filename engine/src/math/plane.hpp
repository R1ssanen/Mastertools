#pragma once

#include "../mtpch.hpp"

namespace mt::math {

    class Plane {
      public:

        f32              SignedDistance(const glm::vec3& Point) const;

        const glm::vec3& GetPoint() const { return m.Point; }

        const glm::vec3& GetNormal() const { return m.Normal; }

        Plane(const glm::vec3& Normal, const glm::vec3& Point = glm::vec3(0.f));

      private:

        struct _M {
            glm::vec3 Point;
            glm::vec3 Normal;
        } m;

        Plane(_M&& Data) : m{ std::move(Data) } { }
    };

    using Frustum = std::vector<Plane>;

} // namespace mt::math

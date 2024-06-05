#pragma once

#include "../mtpch.hpp"

namespace mt::math {

    using aabb_t = std::pair<glm::vec3, glm::vec3>;

    class AABB {
      public:

        const glm::vec3& GetMin() const { return m_Extents.first; }

        const glm::vec3& GetMax() const { return m_Extents.second; }

        const aabb_t&    GetExtents() const { return m_Extents; }

        AABB(f32 MinX, f32 MinY, f32 MinZ, f32 MaxX, f32 MaxY, f32 MaxZ);
        AABB(const glm::vec3& Min, const glm::vec3& Max);
        AABB() = default;

      private:

        aabb_t m_Extents = std::make_pair(glm::vec3(0.f), glm::vec3(0.f));
    };

} // namespace mt::math

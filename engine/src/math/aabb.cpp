#include "aabb.hpp"

#include "../mtpch.hpp"

namespace mt::math {

    AABB::AABB(f32 MinX, f32 MinY, f32 MinZ, f32 MaxX, f32 MaxY, f32 MaxZ)
        : m_Extents(std::make_pair(glm::vec3{ MinX, MinY, MinZ }, glm::vec3{ MaxX, MaxY, MaxZ })) {
    }

    AABB::AABB(const glm::vec3& Min, const glm::vec3& Max) : m_Extents(std::make_pair(Min, Max)) { }

} // namespace mt::math

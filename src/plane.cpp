#include "plane.hpp"

#include <glm/detail/func_geometric.inl>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "mtdefs.hpp"

namespace mt {

    GeneralPlane::GeneralPlane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
        m_normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
        m_d      = -glm::dot(p0, m_normal);
    }

    GeneralPlane::GeneralPlane(const glm::vec3& p, const glm::vec3& n) {
        m_normal = n;
        m_d      = -glm::dot(p, n);
    }

    bool GeneralPlane::inside(const glm::vec4& p) const { return false; }

    f32  GeneralPlane::line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const {
        return 0.f;
        /*glm::vec4 direction    = p1 - p0;

        f32       discriminant = glm::dot(m_normal, direction);
        if (discriminant == 0.f) return false;

        f32 t        = (m_d - glm::dot(m_normal, a)) / discriminant;
        intersection = a + glm::normalize(line) * t;
    */
    }

} // namespace mt

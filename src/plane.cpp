#include "plane.hpp"

#include <cassert>
#include <cfloat>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

#include "mtdefs.hpp"

namespace mt {

    Plane::Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        m_normal = glm::normalize(glm::cross(b - a, c - a));
        m_d      = -glm::dot(a, m_normal);
    }

    Plane::Plane(const glm::vec3& pos, const glm::vec3& normal) {
        assert(glm::length(normal) >= 1 + FLT_EPSILON);

        m_normal = normal;
        m_d      = -glm::dot(pos, normal);
    }

    bool
    Plane::IntersectsLine(const glm::vec3& a, const glm::vec3& b, glm::vec3& intersection) const {
        glm::vec3 line         = b - a;

        f32       discriminant = glm::dot(m_normal, line);
        if (discriminant <= FLT_EPSILON) return false;

        f32 t        = (m_d - glm::dot(m_normal, a)) / discriminant;
        intersection = a + (glm::normalize(line) * t);
        return true;
    }

} // namespace mt

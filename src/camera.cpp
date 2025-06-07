#include "camera.hpp"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>

#include "mtdefs.hpp"

namespace mt {

    void BaseCamera::SetPosition(const glm::vec3& pos) { m_pos = pos; }

    void BaseCamera::SetNearDistance(f32 near) { m_near = near; }

    void BaseCamera::SetFarDistance(f32 far) { m_far = far; }

    void BaseCamera::SetFieldOfView(f32 fov) { m_fov = fov; }

    void BaseCamera::SetAspectRatio(f32 aspect_ratio) { m_aspect = aspect_ratio; }

} // namespace mt

namespace mt {

    DefaultCamera::DefaultCamera(
        const glm::vec3& pos, const glm::vec3& forward, f32 near, f32 far, f32 fov, f32 aspect_ratio
    )
        : BaseCamera(pos, near, far, fov, aspect_ratio), m_view(m_CreateViewMatrix()),
          m_projection(m_CreateProjectionMatrix()) {
        m_near                 = near;
        m_far                  = far;
        m_fov                  = glm::radians(fov);

        glm::vec3 front        = glm::normalize(forward);
        glm::vec3 right        = glm::cross(front, glm::vec3(0.f, 1.f, 0.f));
        glm::vec3 up           = glm::cross(right, front);

        f32       half_fov     = m_fov * 0.5f;
        f32       half_width   = tanf(half_fov * aspect_ratio) * near;
        f32       half_height  = tanf(half_fov) * near;

        glm::vec3 plane_front  = near * front;
        glm::vec3 plane_right  = half_width * right;
        glm::vec3 plane_up     = half_height * up;

        auto      top_left     = pos + plane_front - plane_right + plane_up;
        auto      top_right    = pos + plane_front + plane_right + plane_up;
        auto      bottom_left  = pos + plane_front - plane_right - plane_up;
        auto      bottom_right = pos + plane_front + plane_right - plane_up;

        /*m_frustum.emplace_back(pos + front * near, front);
        m_frustum.emplace_back(pos, top_left, top_right);
        m_frustum.emplace_back(pos, bottom_left, bottom_right);
        m_frustum.emplace_back(pos, top_left, bottom_left);
        m_frustum.emplace_back(pos, top_right, bottom_right);
        m_frustum.emplace_back(pos + front * far, front * -1.f);
        */
    }

    void DefaultCamera::SetPosition(const glm::vec3& pos) {
        m_pos  = pos;
        m_view = m_CreateViewMatrix();
    }

    void DefaultCamera::SetNearDistance(f32 near) {
        m_near       = near;
        m_projection = m_CreateProjectionMatrix();
    }

    void DefaultCamera::SetFarDistance(f32 far) {
        m_far        = far;
        m_projection = m_CreateProjectionMatrix();
    }

    void DefaultCamera::SetFieldOfView(f32 fov) {
        m_fov        = glm::radians(fov);
        m_projection = m_CreateProjectionMatrix();
    }

    void DefaultCamera::SetAspectRatio(f32 aspect_ratio) {
        m_aspect     = aspect_ratio;
        m_projection = m_CreateProjectionMatrix();
    }

    const glm::mat4& DefaultCamera::GetViewMatrix(void) const { return m_view; }

    const glm::mat4& DefaultCamera::GetProjectionMatrix(void) const { return m_projection; }

    glm::mat4        DefaultCamera::m_CreateViewMatrix(void) const {
        return glm::lookAtRH(m_pos, m_pos + forward(), glm::vec3(0.f, 1.f, 0.f));
    }

    glm::mat4 DefaultCamera::m_CreateProjectionMatrix(void) const {
        return glm::perspectiveRH_ZO(m_fov, m_aspect, m_near, m_far);
        // reverse-Z mapping, z -> [1, 0]
        // return glm::perspectiveRH_ZO(m_fov, m_aspect, m_far, m_near);
    }

} // namespace mt

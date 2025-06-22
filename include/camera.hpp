#ifndef MT_CAMERA_HPP_
#define MT_CAMERA_HPP_

#include <cmath>
#include <format>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

#include "mtdefs.hpp"
#include "plane.hpp"

namespace mt {

    class BaseCamera {
      public:

        BaseCamera(void)                                         = delete;

        virtual ~BaseCamera(void)                                = default;

        virtual const glm::mat4& GetViewMatrix(void) const       = 0;

        virtual const glm::mat4& GetProjectionMatrix(void) const = 0;

        virtual void             SetPosition(const glm::vec3& pos);

        const glm::vec3&         orientation() const noexcept { return m_orient; }

        inline const glm::vec3&  GetPosition() { return m_pos; }

        virtual void             SetNearDistance(f32 near);

        virtual void             SetFarDistance(f32 far);

        virtual void             SetFieldOfView(f32 fov);

        f32                      fov() const noexcept { return m_fov; }

        virtual void             SetAspectRatio(f32 aspect_ratio);

        glm::vec3                forward() const {

            f32       yaw     = m_orient.x;
            f32       pitch   = m_orient.y;

            glm::vec3 forward = { std::cos(yaw) * std::cos(pitch), std::sin(pitch),
                                                 std::sin(yaw) * std::cos(pitch) };
            return glm::normalize(forward);
        }

        glm::vec3 right() const { return glm::cross(forward(), glm::vec3(0.f, 1.f, 0.f)); }

        glm::vec3 up() const { return glm::cross(right(), forward()); }

      protected:

        BaseCamera(const glm::vec3& pos, f32 near, f32 far, f32 fov, f32 aspect_ratio)
            : m_orient(glm::vec3(-glm::half_pi<f32>(), 0.f, 0.f)), m_pos(pos), m_near(near),
              m_far(far), m_fov(glm::radians(fov)), m_aspect(aspect_ratio) { }

        glm::vec3 m_orient;
        glm::vec3 m_pos;
        f32       m_near;
        f32       m_far;
        f32       m_fov; // radians
        f32       m_aspect;
    };

} // namespace mt

namespace mt {

    class DefaultCamera : public BaseCamera {
      public:

        DefaultCamera(
            const glm::vec3& pos     = glm::vec3(0.f),
            const glm::vec3& forward = glm::vec3(0.f, 0.f, -1.f), f32 near = 0.1f, f32 far = 50.f,
            f32 fov = 90.f, f32 aspect_ratio = 1440.f / 900.f
        );

        const glm::mat4& GetViewMatrix(void) const override;

        const glm::mat4& GetProjectionMatrix(void) const override;

        void             SetPosition(const glm::vec3& pos) override;

        void             SetNearDistance(f32 near) override;

        void             SetFarDistance(f32 far) override;

        void             SetFieldOfView(f32 fov) override;

        void             SetAspectRatio(f32 aspect_ratio) override;

        void             rotate(f32 dx, f32 dy) {

            constexpr f32 max_pitch = glm::radians(89.f);
            constexpr f32 sens_x = 0.5f, sens_y = 0.5f;

            f32           yaw   = glm::radians(dx * sens_x);
            f32           pitch = glm::radians(dy * sens_y);

            m_orient += glm::vec3(yaw, -pitch, 0.f);
            m_orient.y = std::clamp(m_orient.y, -max_pitch, max_pitch);
            m_view     = m_CreateViewMatrix();
        }

      protected:

        // std::vector<Plane> m_frustum;
        glm::mat4 m_view;
        glm::mat4 m_projection;

      private:

        glm::mat4 m_CreateViewMatrix(void) const;

        glm::mat4 m_CreateProjectionMatrix(void) const;
    };

    inline glm::vec3 screen_to_screen(
        const glm::vec3& p, const glm::mat4& view0, const glm::mat4& view1, const glm::mat4& proj,
        u32 width, u32 height
    ) {
        // Convert screen-space coordinates to normalized device coordinates (NDC)
        glm::vec4 ndc;
        ndc.x                  = (2.0f * p.x) / width - 1.0f;
        ndc.y                  = 1.0f - (2.0f * p.y) / height;
        ndc.z                  = p.z; // Depth remains unchanged
        ndc.w                  = 1.0f;

        // Convert NDC to world-space using the inverse of the old view-projection matrix
        glm::mat4 oldVPInverse = glm::inverse(proj * view0);
        glm::vec4 worldPoint   = oldVPInverse * ndc;
        worldPoint /= worldPoint.w; // Perspective divide

        // Transform world-space point using the new view matrix
        glm::vec4 newViewPoint   = view1 * worldPoint;

        // Convert back to screen-space using the new projection matrix
        glm::vec4 newScreenPoint = proj * newViewPoint;
        newScreenPoint /= newScreenPoint.w; // Perspective divide

        // Convert back to pixel coordinates
        glm::vec3 finalScreenPoint;
        finalScreenPoint.x = (newScreenPoint.x + 1.0f) * 0.5f * width;
        finalScreenPoint.y = (1.0f - newScreenPoint.y) * 0.5f * height;
        finalScreenPoint.z = newScreenPoint.z; // Depth remains unchanged

        return finalScreenPoint;
    }

} // namespace mt

#endif

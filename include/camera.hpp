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
#include <iostream>

#include "mtdefs.hpp"

namespace mt {

    class BaseCamera {
      public:

        BaseCamera(void)                                         = delete;

        virtual ~BaseCamera(void)                                = default;

        virtual const glm::mat4& GetViewMatrix(void) const       = 0;

        virtual const glm::mat4& GetProjectionMatrix(void) const = 0;

        virtual void             SetQuaternion(const glm::quat& quat);

        virtual void             SetPosition(const glm::vec3& pos);

        inline const glm::vec3&  GetPosition() { return m_pos; }

        virtual void             SetNearDistance(f32 near);

        virtual void             SetFarDistance(f32 far);

        virtual void             SetFieldOfView(f32 fov);

        virtual void             SetAspectRatio(f32 aspect_ratio);

      protected:

        BaseCamera(
            const glm::quat& quat, const glm::vec3& pos, f32 near, f32 far, f32 fov,
            f32 aspect_ratio
        )
            : m_quat(quat), m_pos(pos), m_near(near), m_far(far), m_fov(glm::radians(fov)),
              m_aspect(aspect_ratio) { }

        glm::quat m_quat;
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
            f32              aspect_ratio,
            const glm::quat& quat =
                glm::angleAxis(glm::half_pi<float>(), glm::vec3(0.f, 0.f, -1.f)),
            const glm::vec3& pos = glm::vec3(0.f), f32 near = 0.01f, f32 far = 100.f, f32 fov = 90.f
        )
            : BaseCamera(quat, pos, near, far, fov, aspect_ratio), m_view(m_CreateViewMatrix()),
              m_projection(m_CreateProjectionMatrix()) { }

        const glm::mat4& GetViewMatrix(void) const override;

        const glm::mat4& GetProjectionMatrix(void) const override;

        void             SetQuaternion(const glm::quat& quat) override;

        void             SetPosition(const glm::vec3& pos) override;

        void             SetNearDistance(f32 near) override;

        void             SetFarDistance(f32 far) override;

        void             SetFieldOfView(f32 fov) override;

        void             SetAspectRatio(f32 aspect_ratio) override;

      protected:

        glm::mat4 m_view;
        glm::mat4 m_projection;
        // std::unordered_map<std::string, Plane> m_frustum;

      private:

        glm::mat4 m_CreateViewMatrix(void) const;

        glm::mat4 m_CreateProjectionMatrix(void) const;
    };

} // namespace mt

#endif

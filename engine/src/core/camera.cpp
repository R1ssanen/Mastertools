#include "camera.hpp"

#include "../mtpch.hpp"
#include "clipping.hpp"
#include "keys.hpp"
#include "settings.hpp"

namespace mt {

    glm::mat4 Camera::GetMatView() const {
        return glm::lookAtRH(m.Pos, m.Pos + GetForward(), glm::vec3(0.f, 1.f, 0.f));
    }

    glm::mat4 Camera::GetMatProjection() const {
        return glm::perspectiveRH(m.Fov, GetRenderAspectRatio(), m.Near, m.Far);
    }

    glm::vec3 Camera::GetForward() const {
        return glm::yawPitchRoll(m.Angle.x, m.Angle.y, m.Angle.z) * glm::vec4(0.f, 0.f, -1.f, 1.f);
    }

    glm::vec3 Camera::GetRight() const {
        return glm::yawPitchRoll(m.Angle.x, m.Angle.y, m.Angle.z) * glm::vec4(1.f, 0.f, 0.f, 1.f);
    }

    glm::vec3 Camera::GetUp() const {
        return glm::yawPitchRoll(m.Angle.x, m.Angle.y, m.Angle.z) * glm::vec4(0.f, 1.f, 0.f, 1.f);
    }

    void Camera::HandleMovement() {
        glm::vec3 Forward = GetForward(), Right = GetRight();

        if (GetKey(SDL_SCANCODE_W)) { m.Pos += Forward * CAMERA_SPEED; }

        if (GetKey(SDL_SCANCODE_S)) { m.Pos -= Forward * CAMERA_SPEED; }

        if (GetKey(SDL_SCANCODE_A)) { m.Pos -= Right * CAMERA_SPEED; }

        if (GetKey(SDL_SCANCODE_D)) { m.Pos += Right * CAMERA_SPEED; }

        if (GetKey(SDL_SCANCODE_SPACE)) { m.Pos.y -= CAMERA_SPEED; }

        if (GetKey(SDL_SCANCODE_LSHIFT)) { m.Pos.y += CAMERA_SPEED; }
    }

    void Camera::HandleRotation() {
        auto [DeltaMouseX, DeltaMouseY] = GetDeltaMouse();

        f32 DeltaAngleYaw               = static_cast<f32>(DeltaMouseX / GetSettingSensitivity().x),
            DeltaAnglePitch             = static_cast<f32>(DeltaMouseY / GetSettingSensitivity().y);

        f32 NewAnglePitch =
            glm::clamp(m.Angle.y + DeltaAnglePitch, -glm::radians(85.f), glm::radians(85.f));
        f32 NewAngleYaw = m.Angle.x - DeltaAngleYaw;

        m.Angle += glm::vec3(NewAngleYaw, NewAnglePitch, m.Angle.z);
        m.Angle *= 0.5f;
    }

    Camera::Camera(const glm::vec3& Pos, const glm::vec3& Angle, f32 Fov, f32 Near, f32 Far) {

        f32       HalfFov      = glm::radians(Fov * 0.5f);
        f32       HalfHeight   = std::tan(HalfFov) * Near;
        f32       HalfWidth    = HalfHeight * GetRenderAspectRatio();

        glm::vec3 TopLeft      = glm::normalize(glm::vec3(HalfWidth, HalfHeight, -Near)),
                  BottomLeft   = glm::normalize(glm::vec3(HalfWidth, -HalfHeight, -Near)),
                  TopRight     = glm::normalize(glm::vec3(-HalfWidth, HalfHeight, -Near)),
                  BottomRight  = glm::normalize(glm::vec3(-HalfWidth, -HalfHeight, -Near));

        glm::vec3 NormalLeft   = glm::cross(TopLeft, BottomLeft),
                  NormalRight  = glm::cross(BottomRight, TopRight),
                  NormalTop    = glm::cross(TopRight, TopLeft),
                  NormalBottom = glm::cross(BottomLeft, BottomRight);

        math::Frustum Frustum  = {
            math::Plane(glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, -Near)),
            math::Plane(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -Far)),
            math::Plane(NormalLeft),
            math::Plane(NormalRight),
            math::Plane(NormalTop),
            math::Plane(NormalBottom)
        };

        m = _M{ .Frustum = Frustum,
                .Pos     = Pos,
                .Angle   = Angle,
                .Fov     = glm::radians(Fov),
                .Near    = Near,
                .Far     = Far };
    }

    Camera GetDefaultCamera() { return Camera(glm::vec3(0.f), glm::vec3(0.f), 90.f, 0.05f, 100.f); }

} // namespace mt

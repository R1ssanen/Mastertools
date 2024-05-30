#pragma once

#include "clipping.hpp"
#include "mtpch.hpp"

#define CAMERA_SPEED 0.05f

namespace core
{

class Camera
{
  public:
    glm::mat4 GetMatView() const;
    glm::mat4 GetMatProjection() const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    const glm::vec3& GetPos() const { return m.Pos; }
    const glm::vec3& GetAngle() const { return m.Angle; }
    const f32& GetFov() const { return m.Fov; }
    const f32& GetNear() const { return m.Near; }
    const f32& GetFar() const { return m.Far; }
    const Frustum& GetFrustum() const { return m.Frustum; }
    f32 GetInverseFar() const { return 1.f / m.Far; }

    void HandleMovement();
    void HandleRotation();

    static Camera New(const glm::vec3& Pos, const glm::vec3& Angle, f32 Fov, f32 Near, f32 Far);

  private:
    struct _M
    {
        core::Frustum Frustum;
        glm::vec3 Pos, Angle;
        f32 Fov, Near, Far;
    } m;

    Camera(_M&& Data) : m{std::move(Data)} {}
};

Camera GetDefaultCamera();

} // namespace core

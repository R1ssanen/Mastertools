#pragma once

#include "clipping.hpp"
#include "srpch.hpp"

#define CAMERA_SPEED 0.1f

namespace core
{

class Camera
{
  public:
    glm::mat4 GetMatView() const;
    glm::mat4 GetMatProjection(float t_Width, float t_Height) const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    const glm::vec3& GetPos() const { return m.Pos; }
    const glm::vec3& GetAngle() const { return m.Angle; }
    const float& GetFov() const { return m.Fov; }
    const float& GetNear() const { return m.Near; }
    const float& GetFar() const { return m.Far; }
    const Frustum& GetClipFrustum() const { return m.ClipFrustum; }
    const Frustum& GetCullFrustum() const { return m.CullFrustum; }
    float GetInverseFar() const { return 1.f / m.Far; }

    void HandleMovement();
    void HandleRotation();

    static Camera New(const glm::vec3& t_Pos, const glm::vec3& t_Angle, float t_Fov, float t_Near, float t_Far);

  private:
    struct _M
    {
        glm::vec3 Pos, Angle;
        float Fov, Near, Far;
        Frustum ClipFrustum, CullFrustum;
    } m;

    Camera(_M&& t_Data) : m{std::move(t_Data)} {}
};

Camera GetDefaultCamera();

} // namespace core

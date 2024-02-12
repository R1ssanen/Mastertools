#include "camera.hpp"

#include "clipping.hpp"
#include "keys.hpp"
#include "settings.hpp"
#include "srpch.hpp"

namespace core
{

glm::mat4 Camera::GetMatView() const
{
    return glm::lookAtRH(m.Pos, m.Pos + GetForward(), glm::vec3(0.f, 1.f, 0.f));
}

glm::mat4 Camera::GetMatProjection(float t_Width, float t_Height) const
{
    return glm::perspectiveFovRH(m.Fov, t_Width, t_Height, m.Near, m.Far);
}

glm::vec3 Camera::GetForward() const
{
    return glm::yawPitchRoll(m.Angle.x, m.Angle.y, m.Angle.z) * glm::vec4(0.f, 0.f, -1.f, 1.f);
}

glm::vec3 Camera::GetRight() const
{
    return glm::yawPitchRoll(m.Angle.x, m.Angle.y, m.Angle.z) * glm::vec4(1.f, 0.f, 0.f, 1.f);
}

glm::vec3 Camera::GetUp() const
{
    return glm::yawPitchRoll(m.Angle.x, m.Angle.y, m.Angle.z) * glm::vec4(0.f, 1.f, 0.f, 1.f);
}

void Camera::HandleMovement()
{
    glm::vec3 Forward = GetForward(), Right = GetRight();

    if (GetKey(SDL_SCANCODE_W))
    {
        m.Pos += Forward * CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_S))
    {
        m.Pos -= Forward * CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_A))
    {
        m.Pos -= Right * CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_D))
    {
        m.Pos += Right * CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_SPACE))
    {
        m.Pos.y -= CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_LSHIFT))
    {
        m.Pos.y += CAMERA_SPEED;
    }
}

void Camera::HandleRotation()
{
    auto [DeltaMouseX, DeltaMouseY] = GetDeltaMouse();

    float DeltaAngleYaw{static_cast<float>(DeltaMouseX / GetSettingSensitivity().x)},
        DeltaAnglePitch{static_cast<float>(DeltaMouseY / GetSettingSensitivity().y)};

    float NewAnglePitch{glm::clamp(m.Angle.y + DeltaAnglePitch, -glm::radians(85.f), glm::radians(85.f))};
    float NewAngleYaw{m.Angle.x - DeltaAngleYaw};

    m.Angle += glm::vec3(NewAngleYaw, NewAnglePitch, m.Angle.z);
    m.Angle *= 0.5f;
}

Camera Camera::New(const glm::vec3& t_Pos, const glm::vec3& t_Angle, float t_Fov, float t_Near, float t_Far) {
    
    Frustum ClipFrustum = {
        Plane::New(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, t_Near)),
        Plane::New(glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, t_Far))
    };

    float HalfFov = glm::radians(t_Fov * 0.5f);
    float HalfHeight = std::tan(HalfFov) * t_Near;
    float HalfWidth = HalfHeight * (16.f / 9.f);

    glm::vec3 TopLeft = glm::normalize(glm::vec3(HalfWidth, HalfHeight, -t_Near)),
        BottomLeft = glm::normalize(glm::vec3(HalfWidth, -HalfHeight, -t_Near)),
        TopRight = glm::normalize(glm::vec3(-HalfWidth, HalfHeight, -t_Near)),
        BottomRight = glm::normalize(glm::vec3(-HalfWidth, -HalfHeight, -t_Near));

    glm::vec3 NormalLeft = glm::cross(TopLeft, BottomLeft),
        NormalRight = glm::cross(BottomRight, TopRight),
        NormalTop = glm::cross(TopRight, TopLeft),
        NormalBottom = glm::cross(BottomLeft, BottomRight);

    Frustum CullFrustum = {
        Plane::New(glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, -t_Near)),
        Plane::New(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -t_Far)),
        
        Plane::New(NormalLeft),
        Plane::New(NormalRight),
        Plane::New(NormalTop),
        Plane::New(NormalBottom),
    };
    
    return Camera(
        _M{
        .Pos = t_Pos,
        .Angle = t_Angle,
        .Fov = t_Fov,
        .Near = t_Near,
        .Far = t_Far,
        .ClipFrustum = ClipFrustum,
        .CullFrustum = CullFrustum
        }
    );
}

Camera GetDefaultCamera() {
    return Camera::New(glm::vec3(0.f), glm::vec3(0.f), 90.f, 0.05f, 100.f);
}

} // namespace core

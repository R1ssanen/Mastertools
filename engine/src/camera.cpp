#include "camera.hpp"

#include "clipping.hpp"
#include "keys.hpp"
#include "settings.hpp"
#include "srpch.hpp"

namespace core
{

glm::mat4 Camera::GetMatView() const
{
    glm::vec3 Forward{glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) * glm::vec4(0.f, 0.f, -1.f, 1.f)};
    return glm::lookAtRH(m_Pos, m_Pos + Forward, glm::vec3(0.f, 1.f, 0.f));
}

glm::mat4 Camera::GetMatProjection(float t_Width, float t_Height) const
{
    return glm::perspectiveFovRH(m_Fov, t_Width, t_Height, m_Near, m_Far);
}

glm::vec3 Camera::GetForward() const
{
    return glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) * glm::vec4(0.f, 0.f, -1.f, 1.f);
}

glm::vec3 Camera::GetRight() const
{
    return glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) * glm::vec4(1.f, 0.f, 0.f, 1.f);
}

glm::vec3 Camera::GetUp() const
{
    return glm::yawPitchRoll(m_Angle.x, m_Angle.y, m_Angle.z) * glm::vec4(0.f, 1.f, 0.f, 1.f);
}

void Camera::HandleMovement()
{
    glm::vec3 Forward = GetForward(), Right = GetRight();

    if (GetKey(SDL_SCANCODE_W))
    {
        m_Pos += Forward * CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_S))
    {
        m_Pos -= Forward * CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_A))
    {
        m_Pos -= Right * CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_D))
    {
        m_Pos += Right * CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_SPACE))
    {
        m_Pos.y -= CAMERA_SPEED;
    }

    if (GetKey(SDL_SCANCODE_LSHIFT))
    {
        m_Pos.y += CAMERA_SPEED;
    }
}

void Camera::HandleRotation()
{
    auto [DeltaMouseX, DeltaMouseY] = GetDeltaMouse();

    float DeltaAngleYaw{static_cast<float>(DeltaMouseX / GetSettingSensitivity().x)},
        DeltaAnglePitch{static_cast<float>(DeltaMouseY / GetSettingSensitivity().y)};

    float NewAnglePitch{glm::clamp(m_Angle.y + DeltaAnglePitch, -glm::radians(85.f), glm::radians(85.f))};
    float NewAngleYaw{m_Angle.x - DeltaAngleYaw};

    m_Angle += glm::vec3(NewAngleYaw, NewAnglePitch, m_Angle.z);
    m_Angle *= 0.5f;
}

Camera GetDefaultCamera() { return Camera(glm::vec3(0.f), glm::vec3(0.f), 90.f, 0.05f, 100.f); }

} // namespace core

#include "light.hpp"

#include "glm/gtx/fast_square_root.hpp"
#include "mesh.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

namespace core
{

PointLight PointLight::New(const glm::vec3& t_Pos, float t_Intensity)
{
    return PointLight(_M{.Pos = t_Pos, .Intensity = t_Intensity});
}

float PointLight::GetLighting(const Vertex& t_Vertex) const
{
    const float Distance{glm::fastDistance(glm::vec3(t_Vertex.m_Pos), m.Pos)};
    return m.Intensity / (Distance * Distance);
}

} // namespace core

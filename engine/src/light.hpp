#pragma once

#include "mesh.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

namespace core
{

class PointLight
{
  public:
    const glm::vec3& GetPos() const { return m.Pos; }
    const float& GetIntensity() const { return m.Intensity; }

    float GetLighting(const Vertex& t_Vertex) const;

    static PointLight New(const glm::vec3& t_Pos, float t_Intensity);

  private:
    struct _M
    {
        glm::vec3 Pos;
        float Intensity;
    } m;

    PointLight(_M&& t_Data) : m{std::move(t_Data)} {}
};

} // namespace core

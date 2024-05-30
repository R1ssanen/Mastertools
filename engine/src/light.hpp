#pragma once

#include "mesh.hpp"
#include "mtpch.hpp"
#include "vertex.hpp"

namespace core
{

class PointLight
{
  public:
    const glm::vec3& GetPos() const { return m.Pos; }
    const glm::vec3& GetColor() const { return m.Color; }
    const f32& GetIntensity() const { return m.Intensity; }

    f32 GetLighting(const Vertex& Vertex) const;

    static PointLight New(const glm::vec3& Pos, const glm::vec3& Color, f32 Intensity);

  private:
    struct _M
    {
        glm::vec3 Pos, Color;
        f32 Intensity;
    } m;

    PointLight(_M&& Data) : m{std::move(Data)} {}
};

} // namespace core

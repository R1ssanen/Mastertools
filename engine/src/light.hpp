#pragma once

#include "mesh.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

namespace core {

class PointLight {
 public:
  PointLight() = default;
  ~PointLight() = default;

  PointLight(const glm::vec3& t_Pos, float t_Intensity);

  const glm::vec3& GetPos() const { return m_Pos; }
  void SetPos(const glm::vec3& t_Value) { m_Pos = t_Value; }
  const float& GetIntensity() const { return m_Intensity; }

  float GetLighting(const Vertex& t_Vertex) const;

 private:
  glm::vec3 m_Pos{0.f};
  float m_Intensity{1.f};
};


}  // namespace core

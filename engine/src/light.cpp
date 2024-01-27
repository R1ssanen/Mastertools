#include "light.hpp"

#include "glm/gtx/fast_exponential.hpp"
#include "glm/gtx/fast_square_root.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

namespace core {

PointLight::PointLight(const glm::vec3& t_Pos, float t_Intensity)
    : m_Pos{t_Pos}, m_Intensity{t_Intensity} {}

float PointLight::GetLighting(const Vertex& t_Vertex) const {
  /*float InverseDistance2{
      glm::fastPow(glm::fastDistance(glm::vec3(t_Vertex.m_Pos), m_Pos), -2.f)};

  //std::clog << m_Intensity * InverseDistance2 << '\n';
  return m_Intensity * InverseDistance2;*/
  return m_Intensity /
         glm::pow(glm::distance(glm::vec3(t_Vertex.m_Pos), m_Pos), 2.f);
}

}  // namespace core

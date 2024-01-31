#pragma once

#include "clipping.hpp"
#include "srpch.hpp"
#include "camera.hpp"

namespace core {

inline bool WorldspaceBackfaceCull(const Tri& t_Tri, const Camera& Camera) {
  glm::vec3 Normal = Camera.GetMatView() * glm::vec4(glm::cross(glm::vec3(t_Tri[2].m_Normal - t_Tri[0].m_Normal),
                                                      glm::vec3(t_Tri[1].m_Normal - t_Tri[0].m_Normal)), 1.f);

  glm::vec3 View = Camera.GetMatView() * glm::vec4(Camera.GetForward(), 1.f);

    //!Mesh->Texture->IsDoublesided() && 
  return glm::dot(glm::normalize(View), glm::normalize(Normal)) <= 0.f;
}

inline bool ClipspaceBackfaceCull(const Tri& t_Tri) {
    const auto [a, b, c] = t_Tri;
    return (b.m_Pos.x - a.m_Pos.x) * (c.m_Pos.y - a.m_Pos.y) -
             (c.m_Pos.x - a.m_Pos.x) * (b.m_Pos.y - a.m_Pos.y) <= 0.f;
}

}

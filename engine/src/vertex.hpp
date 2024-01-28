#pragma once

#include "srpch.hpp"

#define AMBIENT_INTENSITY 0.05f

namespace core {

class Vertex {
 public:
  ~Vertex() = default;

  Vertex(const glm::vec4& t_Pos = glm::vec4(0.f, 0.f, 0.f, 1.f),
         const glm::vec4& t_Normal = glm::vec4(0.f, 0.f, 0.f, 1.f),
         const glm::vec2& t_UV = glm::vec2(0.f, 0.f),
         float t_Light = AMBIENT_INTENSITY)
      : m_Pos{t_Pos}, m_Normal{t_Normal}, m_UV{t_UV}, m_Light{t_Light} {}

 public:
  glm::vec4 m_Pos{0.f, 0.f, 0.f, 1.f}, m_Normal{0.f};
  glm::vec2 m_UV{0.f};
  float m_Light{0.f};
};

using Tri = std::array<Vertex, 3>;

struct DrawTri {
  Tri m_Tri;
  std::string m_TexName;

  static bool FrontToBack(const DrawTri& t_First, const DrawTri& t_Second) {
    return (t_First.m_Tri[0].m_Pos.z + t_First.m_Tri[1].m_Pos.z +
            t_First.m_Tri[2].m_Pos.z) *
               glm::third<float>() >
           (t_Second.m_Tri[0].m_Pos.z + t_Second.m_Tri[1].m_Pos.z +
            t_Second.m_Tri[2].m_Pos.z) *
               glm::third<float>();
  }

  static bool BackToFront(const DrawTri& t_First, const DrawTri& t_Second) {
    return !FrontToBack(t_First, t_Second);
  }
};

}  // namespace core

#include "culling.hpp"

#include "clipping.hpp"
#include "srpch.hpp"

namespace core {

bool BackfaceCull(const Tri& t_Tri) {
  const auto [a, b, c] = t_Tri;
  return (b.m_Pos.x - a.m_Pos.x) * (c.m_Pos.y - a.m_Pos.y) -
             (c.m_Pos.x - a.m_Pos.x) * (b.m_Pos.y - a.m_Pos.y) <=
         0.f;
}
}  // namespace core

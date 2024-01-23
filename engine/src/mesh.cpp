#include "mesh.hpp"
#include "vertex.hpp"

#include "srpch.hpp"

namespace core {

Mesh::Mesh(const std::vector<Vertex>& t_Vertices,
           const std::vector<unsigned int>& t_Indices,
           const std::string& t_TexName)
    : m_Vertices{t_Vertices}, m_Indices{t_Indices}, m_TexName{t_TexName} {}

const std::vector<Vertex>& Mesh::GetVertices() const {
  return m_Vertices;
}

const std::vector<unsigned int>& Mesh::GetIndices() const {
  return m_Indices;
}

const std::string& Mesh::GetTexName() const {
  return m_TexName;
}

}  // namespace core

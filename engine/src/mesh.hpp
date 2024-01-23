#pragma once

#include "srpch.hpp"
#include "vertex.hpp"

namespace core {

class Mesh {
 public:
  ~Mesh() = default;
  Mesh(const std::vector<Vertex>& t_Vertices,
       const std::vector<unsigned int>& t_Indices,
       const std::string& t_TexName);

  const std::vector<Vertex>& GetVertices() const;
  const std::vector<unsigned int>& GetIndices() const;
  const std::string& GetTexName() const;

 private:
  std::vector<Vertex> m_Vertices;
  std::vector<unsigned int> m_Indices;
  std::string m_TexName;
};

}  // namespace core

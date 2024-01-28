#pragma once

#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core {

class Mesh {
 public:
  ~Mesh() = default;
  Mesh(const std::vector<Vertex>& t_Vertices,
       const std::vector<unsigned int>& t_Indices,
       const texture_t& t_Texture,
       const glm::vec3& t_MassCenter,
       float t_BoundingRadius);

  const std::vector<Vertex>& GetVertices() const;
  const std::vector<unsigned int>& GetIndices() const;

  texture_t Texture;

 private:
  std::vector<Vertex> m_Vertices;
  std::vector<unsigned int> m_Indices;

  glm::vec3 m_MassCenter{0.f};
  float m_BoundingRadius{0.f};
};

using mesh_vector_t = std::vector<std::shared_ptr<Mesh>>;

mesh_vector_t LoadMeshOBJ(const std::string& t_Directory,
                          const std::string& t_Name,
                          bool t_CullBackfaces = true);

}  // namespace core

#include "mesh.hpp"

#include "OBJ_Loader.h"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core {

Mesh::Mesh(const std::vector<Vertex>& t_Vertices,
           const std::vector<unsigned int>& t_Indices,
           const texture_t& t_Texture,
           const glm::vec3& t_MassCenter,
           float t_BoundingRadius)
    : Texture{t_Texture},
      m_Vertices{t_Vertices},
      m_Indices{t_Indices},
      m_MassCenter{t_MassCenter},
      m_BoundingRadius{t_BoundingRadius} {}

const std::vector<Vertex>& Mesh::GetVertices() const {
  return m_Vertices;
}

const std::vector<unsigned int>& Mesh::GetIndices() const {
  return m_Indices;
}

mesh_vector_t LoadMeshOBJ(const std::string& t_Directory,
                          const std::string& t_Name,
                          bool t_CullBackfaces) {
  objl::Loader Loader;

  if (!Loader.LoadFile(t_Directory + t_Name)) [[unlikely]] {
    throw std::invalid_argument("Asset '" + t_Directory + t_Name +
                                "' could not be loaded.");
  }

  mesh_vector_t Meshes;

  for (const objl::Mesh& ObjlMesh : Loader.LoadedMeshes) {
    std::vector<Vertex> Vertices;
    glm::vec3 VertexSum{0.f};

    for (const objl::Vertex& Vertex : ObjlMesh.Vertices) {
      Vertices.push_back(core::Vertex(
          glm::vec4(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z,
                    1.f),
          glm::vec4(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z, 1.f),
          glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y)));

      VertexSum +=
          glm::vec3(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z);
    }

    glm::vec3 MassCenter{VertexSum / static_cast<float>(Vertices.size())};

    float MaxDistance{0.f};
    for (const Vertex& Vertex : Vertices) {
      float Distance{glm::distance(MassCenter, glm::vec3(Vertex.m_Pos))};
      if (Distance > MaxDistance) {
        MaxDistance = Distance;
      }
    }

    const bool TextureExists{!ObjlMesh.MeshMaterial.map_Kd.empty()};
    const bool Transparent{ObjlMesh.MeshMaterial.d != 1.f};

    texture_t Texture{
        TextureExists ? NewTexture(t_Directory + ObjlMesh.MeshMaterial.map_Kd,
                                   Transparent, t_CullBackfaces)
                      : GetDefaultTexture()};

    Meshes.push_back(std::make_shared<Mesh>(Vertices, ObjlMesh.Indices, Texture,
                                            MassCenter, MaxDistance));
  }

  return Meshes;
}

}  // namespace core

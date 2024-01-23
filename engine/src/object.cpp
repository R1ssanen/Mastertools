#include "object.hpp"

#include "OBJ_Loader.h"
#include "mesh.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core {

Object::Object(const std::vector<Mesh>& t_Meshes,
               const glm::vec3& t_Pos,
               const glm::vec3& t_Angle,
               const glm::vec3& t_Scale)
    : m_Meshes{t_Meshes}, m_Pos{t_Pos}, m_Angle{t_Angle}, m_Scale{t_Scale} {}

glm::mat4 Object::GetScale() const {
  return glm::scale(glm::mat4(1.f), m_Scale);
}

glm::mat4 Object::GetRotation(float DeltaTime) const {
  return glm::orientate4(m_Angle + DeltaTime);
}

glm::mat4 Object::GetTranslation() const {
  return glm::translate(glm::mat4(1.f), m_Pos);
}

const std::vector<Mesh>& Object::GetMeshes() const {
  return m_Meshes;
}

Object LoadAssetOBJ(const std::string& t_Path) {
  objl::Loader Loader;
  if (!Loader.LoadFile(t_Path)) [[unlikely]] {
    throw std::invalid_argument("Asset path '" + t_Path +
                                "' could not be found.");
  }

  std::vector<Mesh> MeshesOut;

  for (const objl::Mesh& Mesh : Loader.LoadedMeshes) {
    std::vector<Vertex> Vertices;
    for (const objl::Vertex& Vertex : Mesh.Vertices) {
      Vertices.push_back(core::Vertex(
          glm::vec4(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z,
                    1.f),
          glm::vec4(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z, 1.f),
          glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y)));
    }

    const bool NoTexture{Mesh.MeshMaterial.map_Kd.empty()};

    MeshesOut.push_back(core::Mesh(Vertices, Mesh.Indices,
                                   NoTexture
                                       ? "../../../builtins/untextured.png"
                                       : Mesh.MeshMaterial.map_Kd));
  }

  return Object(MeshesOut, glm::vec3(0.f, 0.f, -1.5f), glm::vec3(0.f),
                glm::vec3(1.f));
}
}  // namespace core

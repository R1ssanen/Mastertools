#include "object.hpp"

#include "OBJ_Loader.h"
#include "mesh.hpp"
#include "rapidobj.hpp"
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

glm::mat4 Object::GetRotation(float t_DeltaTime) const {
  return glm::orientate4(m_Angle + t_DeltaTime);
}

glm::mat4 Object::GetTranslation() const {
  return glm::translate(glm::mat4(1.f), m_Pos);
}

const std::vector<Mesh>& Object::GetMeshes() const {
  return m_Meshes;
}

std::vector<Mesh> LoadMeshOBJ(const std::string& t_Directory,
                              const std::string& t_Name) {
  objl::Loader Loader;

  if (!Loader.LoadFile(t_Directory + t_Name)) [[unlikely]] {
    throw std::invalid_argument("Asset '" + t_Directory + t_Name +
                                "' could not be loaded.");
  }

  std::vector<Mesh> Meshes;

  for (const objl::Mesh& Mesh : Loader.LoadedMeshes) {
    std::vector<Vertex> Vertices;

    for (const objl::Vertex& Vertex : Mesh.Vertices) {
      Vertices.push_back(core::Vertex(
          glm::vec4(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z,
                    1.f),
          glm::vec4(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z, 1.f),
          glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y)));
    }

    const bool TextureExists{!Mesh.MeshMaterial.map_Kd.empty()};

    Meshes.push_back(core::Mesh(Vertices, Mesh.Indices,
                                TextureExists
                                    ? t_Directory + Mesh.MeshMaterial.map_Kd
                                    : DEFAULT_TEXTURE));
  }

  return Meshes;
}

Object rapidobj_LoadAssetOBJ(const std::string& t_Directory,
                             const std::string& t_Name) {
  namespace robj = rapidobj;

  robj::Result Result{robj::ParseFile(
      t_Directory + t_Name, robj::MaterialLibrary::SearchPaths(
                                {t_Directory, t_Directory + "textures/"}))};

  if (Result.error) [[unlikely]] {
    throw std::invalid_argument("Asset file '" + t_Name +
                                "' could not be loaded.");
  }

  if (!robj::Triangulate(Result)) {
    throw std::runtime_error("Could not triangulate model file '" + t_Name +
                             "'.");
  }

  std::vector<Mesh> Meshes;

  for (const robj::Shape& Shape : Result.shapes) {
    std::vector<unsigned int> Indices;
    std::vector<Vertex> Vertices;

    for (const robj::Index& Index : Shape.mesh.indices) {
      robj::Array<float>& Positions{Result.attributes.positions};
      robj::Array<float>& Normals{Result.attributes.normals};
      robj::Array<float>& Texcoords{Result.attributes.texcoords};

      glm::vec4 Pos{Positions[Index.position_index],
                    Positions[Index.position_index + 1],
                    Positions[Index.position_index + 2], 1.f};
      glm::vec4 Normal{Normals[Index.normal_index],
                       Normals[Index.normal_index + 1],
                       Normals[Index.normal_index + 2], 1.f};
      glm::vec2 UV{Texcoords[Index.texcoord_index],
                   Texcoords[Index.texcoord_index + 1]};

      Vertices.push_back(Vertex(Pos, Normal, UV));
      Indices.push_back(Index.position_index);
    }

    if (!Shape.mesh.material_ids.empty()) {
      robj::Material Material{Result.materials[Shape.mesh.material_ids[0]]};
      Meshes.push_back(Mesh(Vertices, Indices, Material.diffuse_texname));
    }

    else {
      Meshes.push_back(Mesh(Vertices, Indices, DEFAULT_TEXTURE));
    }
  }

  return Object(Meshes);
}

}  // namespace core

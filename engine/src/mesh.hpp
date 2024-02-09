#pragma once

#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core
{

class Mesh
{
  public:
    ~Mesh() = default;
    Mesh(const std::vector<Vertex>& t_Vertices, const std::vector<unsigned>& t_Indices, const texture_t& t_Texture);

    const std::vector<Vertex>& GetVertices() const;
    const std::vector<unsigned>& GetIndices() const;

    texture_t Texture;

  private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned> m_Indices;

    glm::vec3 m_MassCenter{0.f};
    float m_BoundingRadius{0.f};
};

using mesh_t = std::shared_ptr<Mesh>;
using mesh_vector_t = std::vector<mesh_t>;

mesh_vector_t LoadAsset(const std::string& t_Directory, const std::string& t_Name, bool t_IsMipmapped,
                        bool t_IsDoublesided);

} // namespace core

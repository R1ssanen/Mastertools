#pragma once

#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core
{

class Mesh
{
  public:
    const vertex_vector_t& GetVertices() const { return m.Vertices; }
    const index_vector_t& GetIndices() const { return m.Indices; }
    const glm::vec3& GetMassCenter() const { return m.MassCenter; }
    const float& GetBoundingRadius() const { return m.BoundingRadius; }
    const uint32_t& GetTextureID() const { return m.TextureID; }

    static Mesh New(const vertex_vector_t& t_Vertices, const index_vector_t& t_Indices, uint32_t t_TextureID);

  private:
    struct _M
    {
        vertex_vector_t Vertices;
        index_vector_t Indices;
        glm::vec3 MassCenter;
        float BoundingRadius;
        uint32_t TextureID;
    } m;

    Mesh(_M&& t_Data) : m{std::move(t_Data)} {}
};

using mesh_t = std::shared_ptr<Mesh>;
using mesh_vector_t = std::vector<mesh_t>;

mesh_vector_t LoadAsset(const std::string& t_Directory, const std::string& t_Name, bool t_IsMipmapped,
                        bool t_IsDoublesided);

} // namespace core

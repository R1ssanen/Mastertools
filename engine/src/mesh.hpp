#pragma once

#include "mtpch.hpp"
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
    const f32& GetBoundingRadius() const { return m.BoundingRadius; }
    const u32& GetTextureID() const { return m.TextureID; }

    Mesh(const Mesh& Other) { m = Other.m; }
    static std::shared_ptr<Mesh> New(const vertex_vector_t& Vertices, const index_vector_t& Indices, u32 TextureID);

  private:
    struct _M
    {
        vertex_vector_t Vertices;
        index_vector_t Indices;
        glm::vec3 MassCenter;
        f32 BoundingRadius;
        u32 TextureID;
    } m;

    explicit Mesh(_M&& Data) : m{std::move(Data)} {}
};

using mesh_t = std::shared_ptr<Mesh>;
using mesh_vector_t = std::vector<mesh_t>;

mesh_vector_t LoadAsset(const std::filesystem::path& Path, b8 IsMipmapped, b8 IsDoublesided);
mesh_t LoadAsset(const std::string& Path, u32 TextureID);

std::vector<glm::vec2> GetConvexHull(const vertex_vector_t& Vertices);

} // namespace core

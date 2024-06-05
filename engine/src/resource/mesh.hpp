#pragma once

#include "../core/vertex.hpp"
#include "../math/aabb.hpp"
#include "../mtpch.hpp"
#include "texture.hpp"

namespace mt {

    class Mesh {
      public:

        const vertex_vector_t& GetVertices() const { return m.Vertices; }

        const index_vector_t&  GetIndices() const { return m.Indices; }

        const math::AABB&      GetAABB() const { return m.AABB; }

        const glm::vec3&       GetMassCenter() const { return m.MassCenter; }

        const f32&             GetBoundingRadius() const { return m.BoundingRadius; }

        const u32&             GetTextureID() const { return m.TextureID; }

        Mesh(const vertex_vector_t& Vertices, const index_vector_t& Indices, u32 TextureID);

      private:

        struct _M {
            vertex_vector_t Vertices;
            index_vector_t  Indices;
            math::AABB      AABB;
            glm::vec3       MassCenter;
            f32             BoundingRadius;
            u32             TextureID;
        } m;
    };

    using mesh_t        = std::shared_ptr<Mesh>;
    using mesh_vector_t = std::vector<mesh_t>;

    mesh_vector_t LoadAsset(const std::filesystem::path& Path, b8 IsMipmapped, b8 IsDoublesided);
    mesh_t        LoadAsset(const std::string& Path, u32 TextureID);

} // namespace mt

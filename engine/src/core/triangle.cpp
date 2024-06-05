#include "triangle.hpp"

#include "../mtpch.hpp"
#include "../resource/mesh.hpp"
#include "../resource/shader.hpp"
#include "vertex.hpp"

namespace mt {

    std::pair<glm::vec2, glm::vec2> Triangle::GetBoundingBox() const {
        auto [MinX, MaxX] =
            std::minmax({ m_Vertices[0].Pos.x, m_Vertices[1].Pos.x, m_Vertices[2].Pos.x });
        auto [MinY, MaxY] =
            std::minmax({ m_Vertices[0].Pos.y, m_Vertices[1].Pos.y, m_Vertices[2].Pos.y });

        return std::pair<glm::vec2, glm::vec2>{ glm::vec2(MinX, MinY), glm::vec2(MaxX, MaxY) };
    }

    void Triangle::CalculateAverageDepth() {
        m_AverageDepth =
            (m_Vertices[0].Pos.z + m_Vertices[1].Pos.z + m_Vertices[2].Pos.z) * glm::third<f32>();
    }

    triangle_vector_t ConstructTriangles(mesh_t Mesh, const vertex_vector_t& Vertices) {
        triangle_vector_t Triangles;

        index_vector_t    Indices = Mesh->GetIndices();
        texture_t         Texture = GetTexture(Mesh->GetTextureID());

        for (u64 ID = 0; ID < Indices.size(); ID += 3) {
            Triangles.emplace_back(
                std::array<Vertex, 3>{ Vertices[ID], Vertices[ID + 1], Vertices[ID + 2] },
                Texture->IsTransparent() ? TransparentSTD : OpaqueSTD, Texture->GetID(), 1.f
            );
        }

        return Triangles;
    }

} // namespace mt

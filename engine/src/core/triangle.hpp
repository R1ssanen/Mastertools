#pragma once

#include "../mtpch.hpp"
#include "../resource/mesh.hpp"
#include "../resource/shader.hpp"
#include "vertex.hpp"

namespace mt {

    //  using Triangle = std::array<Vertex, 3>;

    //    std::pair<glm::vec2, glm::vec2> GetBoundingBox(const Triangle& Tri);

    struct Triangle {
        std::array<Vertex, 3>           m_Vertices;
        shader_t                        m_Shader;
        u32                             m_TextureID;
        f32                             m_Alpha;

        std::pair<glm::vec2, glm::vec2> GetBoundingBox() const;
        f32                             m_AverageDepth;
        void                            CalculateAverageDepth();

        b8 operator<(const Triangle& Other) const { return m_AverageDepth < Other.m_AverageDepth; }

        b8 operator>(const Triangle& Other) const { return m_AverageDepth > Other.m_AverageDepth; }
    };

    using triangle_vector_t = std::vector<Triangle>;

    triangle_vector_t ConstructTriangles(mesh_t Mesh, const vertex_vector_t& Vertices);

} // namespace mt

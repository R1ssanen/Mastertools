#pragma once

#include "settings.hpp"
#include "mtpch.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace core
{

struct Vertex
{
    glm::vec4 m_Pos, m_Normal;
    glm::vec3 m_LightColor;
    glm::vec2 m_UV;
    f32 m_Light;
};

using vertex_vector_t = std::vector<Vertex>;
using index_vector_t = std::vector<u32>;

} // namespace core

#pragma once

#include "../mtpch.hpp"
#include "../resource/shader.hpp"
#include "../resource/texture.hpp"
#include "settings.hpp"

namespace mt {

    struct Vertex {
        glm::vec4 Pos;
        glm::vec4 Normal = glm::vec4(0.f);
        glm::vec2 UV     = glm::vec2(0.f);
        f32       Light  = 0.f;
    };

    using vertex_vector_t = std::vector<Vertex>;
    using index_vector_t  = std::vector<u32>;

} // namespace mt

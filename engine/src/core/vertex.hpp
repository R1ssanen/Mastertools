#pragma once

#include "../mtpch.hpp"
#include "../resource/shader.hpp"
#include "../resource/texture.hpp"
#include "settings.hpp"

namespace mt {

    struct Vertex {
        glm::vec4 Pos, Normal;
        glm::vec2 UV;
        f32       Light;
    };

    using vertex_vector_t = std::vector<Vertex>;
    using index_vector_t  = std::vector<u32>;

} // namespace mt

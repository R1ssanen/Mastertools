#ifndef MT_SHADER_HPP_
#define MT_SHADER_HPP_

#include <cassert>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include "buffers/buffer.hpp"
#include "mtdefs.hpp"
#include "texture.hpp"

#define MVATTRIB(type, name, voffset)     type& name = *(type*)(attribs + offset + voffset);

#define MFGETATTRIB(type, vertex, offset) *((type*)(attribs[vertex]) + offset)

#define MFATTRIB(type, name, offset)                                                               \
    inline type name(void) const {                                                                 \
        return glm::dot(                                                                           \
            glm::vec3(                                                                             \
                MFGETATTRIB(type, 0, offset), MFGETATTRIB(type, 1, offset),                        \
                MFGETATTRIB(type, 2, offset)                                                       \
            ),                                                                                     \
            barycoord                                                                              \
        );                                                                                         \
    }

namespace mt {

    class VertexShaderBase {
      public:

        virtual void operator()() = 0;

        f32*         attribs;
        u64          offset;
        u32          id;
    };

    class FragShaderBase {
      public:

        virtual void operator()(u32& out) const = 0;

        glm::vec3    normal;
        glm::vec3    barycoord;
        glm::vec3    pos;
        f32*         attribs[3];
        u64          loc;
        u32          id;
    };

} // namespace mt

#include <iostream>

namespace mt {

    class StdForwardVertex1 : public VertexShaderBase {
      public:

        void operator()() override {
            auto& p = pos();
            p       = transform * p;
        }

        glm::vec4& pos() { return *(glm::vec4*)(attribs + offset); }

        glm::mat4  transform;
    };

    class StdForwardFrag1 : public FragShaderBase {
      public:

        void operator()(u32& out) const override {

            glm::vec3 to    = screen_to_world(pos.x, pos.y, pos.z, width, height, inv_view_proj);
            glm::vec3 d     = glm::reflect(to, normal);

            auto [face, uv] = sample_cubemap(d);
            out             = (*cubemap)[face][uv];
        }

        MFATTRIB(f32, z, 2)

        glm::mat4          inv_view_proj;
        cubemap_texture_t* cubemap;
        f32*               depth_buffer;
        u32                width, height;
    };

} // namespace mt

#endif

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

        glm::mat4    inv_view_proj;
        glm::vec3    world_normal;
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

            /*if (out != 0xdeadbeef) {
                out = 0xff0000ff;
                return;
            }*/

            /*glm::vec3 world_pos =
                screen_to_world_no_z(pos.x, pos.y, inv_w_2, inv_h_2, inv_view_proj);
            glm::vec3 reflection = glm::reflect(world_pos, world_normal);
            //glm::vec3 refraction = glm::refract(world_pos, world_normal, 1.4f);

            auto [face, uv]      = sample_cubemap(reflection);
            out                  = (*cubemap)[face][uv];
            */

            out = id;
            //  out = glm::packUnorm4x8(glm::vec4(pos.z));
            //  out = glm::packUnorm4x8(glm::vec4(1.f, barycoord));
        }

        cubemap_texture_t* cubemap;
        f32*               depth_buffer;
        u32                width, height;
        f32                inv_w_2, inv_h_2;
    };

} // namespace mt

#endif

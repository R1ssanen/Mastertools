#ifndef MT_SHADER_HPP_
#define MT_SHADER_HPP_

#include <cassert>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include "aabb.hpp"
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

        virtual glm::vec4 operator()() = 0;

        const f32*        attribs;
        u64               offset;
        u32               id;
    };

    class FragShaderBase {
      public:

        virtual void operator()(u32& out) const = 0;

        glm::mat4    inv_view_proj;
        glm::vec3    world_normal;
        glm::vec3    barycoord;
        glm::vec3    pos;

        const f32*   attribs[3];
        u64          loc;
        u32          id;
        f32          w;
    };

} // namespace mt

#include <iostream>

namespace mt {

    class StdForwardVertex1 : public VertexShaderBase {
      public:

        glm::vec4 operator()() override {
            glm::vec3 p = pos();
            return transform * glm::vec4(p, 1.f);
        }

        glm::vec3 pos() { return *(glm::vec3*)(attribs + offset); }

        glm::vec2 uv() { return *(glm::vec2*)(attribs + offset + 3); }

        glm::vec3 normal() { return *(glm::vec3*)(attribs + offset + 5); }

        glm::mat4 transform;
    };

    class StdForwardFrag1 : public FragShaderBase {
      public:

        void operator()(u32& out) const override {
            glm::vec2 uv         = glm::vec2(u(), 1.f - v());
            auto      texel      = unpack((*texture)[uv]);

            glm::vec3 frag_world = screen_to_world(pos, inv_w_2, inv_h_2, inv_view_proj);
            glm::vec3 reflection = glm::reflect(frag_world - camera_world, world_normal);

            auto [face, cuv]     = sample_cubemap(reflection);
            auto cubemap_texel   = unpack((*cubemap)[face][cuv]);

            // out                  = pack(mt::average(cubemap_texel, texel));
            out                  = pack(mt::lerp(cubemap_texel, texel, texel.a));
        }

        MFATTRIB(f32, u, 3)
        MFATTRIB(f32, v, 4)

        AABB*              cubemap_aabb;
        glm::mat4          light_view;
        glm::mat4          projection;
        glm::mat4          rotation;

        glm::vec3          camera_world;

        Texture*           texture;

        cubemap_texture_t* cubemap;
        f32*               depth_buffer;
        f32*               shadowmap;
        u32                width, height;
        f32                inv_w_2, inv_h_2;
        f32                inv_far;
    };

    class ShadowFrag : public FragShaderBase {

        void operator()(u32& out) const override { }
    };

} // namespace mt

#endif

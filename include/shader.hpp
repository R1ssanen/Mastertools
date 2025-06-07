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
        f32          w;
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

            // f32 sz = shadowmap[u32(pos.y) * width + u32(pos.x)];
            // out    = glm::packUnorm4x8(glm::vec4(sz));
            // return;

            f32       ndc_x      = pos.x * inv_w_2 - 1.f;
            f32       ndc_y      = 1.f - pos.y * inv_h_2;
            f32       ndc_z      = pos.z;

            glm::vec4 frag_world = inv_view_proj * glm::vec4(ndc_x, ndc_y, ndc_z, 1.f);
            frag_world /= frag_world.w;

            glm::vec4 ls_clip = projection * light_view * frag_world;
            ls_clip /= ls_clip.w;

            ndc_x = 0.5f + ls_clip.x * 0.5f;
            ndc_y = 0.5f - ls_clip.y * 0.5f;

            if (glm::clamp(ndc_x, 0.f, 1.f) != ndc_x) {
                out = id;
                return;
            }
            if (glm::clamp(ndc_y, 0.f, 1.f) != ndc_y) {
                out = id;
                return;
            }

            f32 shadowmap_z = shadowmap[u32(ndc_y * height) * width + u32(ndc_x * width)];

            // out             = glm::packUnorm4x8(glm::vec4(shadowmap_z));
            //  return;

            f32 bias        = 0.001f;
            if (ls_clip.z > shadowmap_z + bias) out = 0;
            else
                out = id;

            // f32 z                 = shadowmap[u32(pos.y) * width + u32(pos.x)];
            // out                   = glm::packUnorm4x8(glm::vec4(z / 100.f));
            //    out                   = glm::packUnorm4x8(glm::vec4(shadowmap_z / 100.f));
        }

        glm::mat4          light_view;
        glm::mat4          projection;

        cubemap_texture_t* cubemap;
        f32*               depth_buffer;
        f32*               shadowmap;
        u32                width, height;
        f32                inv_w_2, inv_h_2;
    };

    class ShadowFrag : public FragShaderBase {

        void operator()(u32& out) const override { }
    };

} // namespace mt

#endif

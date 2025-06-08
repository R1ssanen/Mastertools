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

            if (out != 0xdeadbeef) {
                out = 0xff0000ff;
                return;
            }

            glm::vec3 frag_world = screen_to_world(pos, inv_w_2, inv_h_2, inv_view_proj);
            glm::vec4 ls_clip    = projection * light_view * glm::vec4(frag_world, 1.f);
            ls_clip /= ls_clip.w;

            f32 ndc_x = 0.5f + ls_clip.x * 0.5f;
            f32 ndc_y = 0.5f - ls_clip.y * 0.5f;

            if (glm::clamp(ndc_x, 0.f, 1.f) != ndc_x) {
                out = id;
                return;
            }
            if (glm::clamp(ndc_y, 0.f, 1.f) != ndc_y) {
                out = id;
                return;
            }

            auto mul = [](u32 color, f32 s) {
                u8* comps = (u8*)&color;
                comps[0] *= s;
                comps[1] *= s;
                comps[2] *= s;
                comps[3] *= s;
                return *(u32*)comps;
            };

            f32 shadowmap_z = shadowmap[u32(ndc_y * height) * width + u32(ndc_x * width)];
            f32 bias        = 0.001f;

            if (ls_clip.z > shadowmap_z + bias) out = mul(id, 0.5f);

            else
                out = id;
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

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

        inline u32 mix(u32 a, u32 b, f32 s) const {
            auto c0 = glm::unpackUnorm4x8(a), c1 = glm::unpackUnorm4x8(b);
            return glm::packUnorm4x8(c0 * s + c1 * (1.f - s));
        }

        inline u32 mul(u32 color, f32 s) const {
            u8* comps = (u8*)&color;
            comps[0] *= s;
            comps[1] *= s;
            comps[2] *= s;
            comps[3] *= s;
            return *(u32*)comps;
        }

#if 0
        void operator()(u32& out) const override {
            f32 z           = pos.z / w * inv_far;
            // out   = glm::packUnorm4x8(glm::vec4(1.f - z));
            // return;

            glm::vec2 uv    = glm::vec2(u(), 1.f - v());
            u32       texel = (*texture)[uv];

            out = mul(texel, 0.5f - 0.5f * z);
            return;

            glm::vec3 frag_world = screen_to_world(pos, inv_w_2, inv_h_2, inv_view_proj);
            glm::vec3 reflection = glm::reflect(frag_world - camera_world, world_normal);
            auto [face, cuv]     = sample_cubemap(reflection);
            u32 cubemap_texel    = (*cubemap)[face][cuv];

            out                  = mix(texel, cubemap_texel, 0.4f);

#    if 0
            glm::vec3 frag_world = screen_to_world(pos, inv_w_2, inv_h_2, inv_view_proj);
            glm::vec4 ls_clip    = projection * light_view * glm::vec4(frag_world, 1.f);
            ls_clip /= ls_clip.w;

            f32 ndc_x = 0.5f + ls_clip.x * 0.5f;
            f32 ndc_y = 0.5f - ls_clip.y * 0.5f;

            /*if (glm::clamp(ndc_x, 0.f, 1.f) != ndc_x) {
                out = id;
                return;
            }
            if (glm::clamp(ndc_y, 0.f, 1.f) != ndc_y) {
                out = id;
                return;
            }*/

            f32 shadowmap_z = shadowmap[u32(ndc_y * height) * width + u32(ndc_x * width)];
            f32 bias        = 0.001f;

            if (ls_clip.z > shadowmap_z + bias) out = mul(id, 0.5f);
            else
                out = id;

            glm::vec3 frag_world = screen_to_world(pos, inv_w_2, inv_h_2, inv_view_proj);
            glm::vec3 reflection = glm::reflect(frag_world - camera_world, world_normal);
            auto [face, uv]      = sample_cubemap(reflection);
            out                  = (*cubemap)[face][uv];
            return;

            auto pack32 = [](const auto& v) -> u32 {
                // rgba -> abgr
                return glm::packUnorm4x8({ 1.f, v.z, v.y, v.x });
            };
#    endif
        }
#else
        void operator()(u32& out) const override {

            /*
            glm::vec2 uv    = glm::vec2(u(), 1.f - v());
            u32       texel = (*texture)[uv];
            out             = texel;
            return;
            */

            // glm::vec3 normal     = rotation * glm::vec4(nx(), ny(), nz(), 0.f);
            glm::vec3 frag_world =
                screen_to_world(pos.x, pos.y, 1.f, inv_w_2, inv_h_2, inv_view_proj);
            // glm::vec3 reflection = glm::reflect(frag_world - camera_world, normal);
            auto [face, cuv] = sample_cubemap_parallax_corrected(
                frag_world, frag_world - camera_world, *cubemap_aabb
            );
            u32 cubemap_texel = (*cubemap)[face][cuv];
            out               = cubemap_texel;
            // out                  = mix(texel, cubemap_texel, 0.4f);
        }
#endif
        MFATTRIB(f32, u, 3)
        MFATTRIB(f32, v, 4)

        MFATTRIB(f32, nx, 5)
        MFATTRIB(f32, ny, 6)
        MFATTRIB(f32, nz, 7)

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

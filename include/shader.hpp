#ifndef MT_SHADER_HPP_
#define MT_SHADER_HPP_

#include <cassert>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include "buffers/buffer.hpp"
#include "mtdefs.hpp"

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

        glm::vec3    barycoord;
        glm::vec2    pos;
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
            //
            pos() = transform * pos();
        }

        glm::vec4& pos() { return *(glm::vec4*)(attribs + offset); }

        glm::mat4  transform;
    };

    class StdForwardFrag1 : public FragShaderBase {
      public:

        void operator()(u32& out) const override {

            /*if (out != 0) {
                auto c = (u8*)(&out);
                c[1]   = 0;
                c[2]   = 0;
                c[3]   = std::min(c[3] + 20, 0xff);
                return;
            }*/

            f32  d      = z();
            f32& d_curr = depth_buffer[loc];

            if (d_curr > d) return;
            else
                d_curr = d;

            out = id;
            // out = glm::packUnorm4x8(glm::vec4(1.f, d, d, d));
            // out = glm::packUnorm4x8(glm::vec4(1.f, barycoord));
        }

        MFATTRIB(f32, z, 2)

        f32* depth_buffer;
        u32  width, height;
    };

} // namespace mt

#endif

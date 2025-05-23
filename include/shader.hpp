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
        f32*         attribs[3];
        u64          loc;
        u32          id;
    };

} // namespace mt

#include <iostream>

namespace mt {

    class StdForwardVertex1 : public VertexShaderBase {
      public:

        void       operator()() override { pos() = transform * pos(); }

        glm::vec4& pos() { return *(glm::vec4*)(attribs + offset); }

        glm::mat4  transform;
    };

    class StdForwardFrag1 : public FragShaderBase {
      public:

        void operator()(u32& out) const override {

            f32& depth = depth_buffer[loc];
            f32  d     = z();
            if (depth > d) return;

            depth = d;
            // out   = glm::packUnorm4x8(glm::vec4(d));
            out   = glm::packUnorm4x8(glm::vec4(1.f, barycoord));
        }

        MFATTRIB(f32, z, 2)

        u32*         colors;
        f32*         depth_buffer;
        Buffer<f32>* db;
    };

} // namespace mt

#endif

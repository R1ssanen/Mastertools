#ifndef MT_FRAMEBUFFER_HPP_
#define MT_FRAMEBUFFER_HPP_

#include "../barycentric.hpp"
#include "../mtdefs.hpp"
#include "buffer.hpp"
#include "ib.hpp"

//
#include "../shader.hpp"

namespace mt {

    enum BufferType {
        BCOLOR = 0x0001,
        BDEPTH = 0x0010,
    };

    class Framebuffer {
      public:

        Framebuffer(u32 width, u32 height)
            : m_color(Buffer<u32>(width, height)), m_depth(Buffer<f32>(width, height)),
              m_width(width), m_height(height), m_pitch(width * sizeof(u32)) { }

        ~Framebuffer() = default;

        void RenderElements(ElementBuffer& ebo, VertexShaderBase& vs, FragShaderBase& fs);

        void RenderLine(const glm::vec4& v0, const glm::vec4& v1, u32 color);

        const Buffer<u32>& GetColorBuffer(void) const { return m_color; }

        const Buffer<f32>& GetDepthBuffer(void) const { return m_depth; }

        u32                GetPitch(void) const { return m_pitch; }

        void               Clear(u32 buffers) {
            if (buffers & BCOLOR) m_color.Memset(0);
            if (buffers & BDEPTH) m_depth.Memset(1);
        }

        bool wireframe      = false;
        bool cull_backfaces = true;

      private:

        Buffer<u32> m_color;
        Buffer<f32> m_depth;
        u64         m_width;
        u64         m_height;
        u32         m_pitch;

      private:

        void RenderTriangle(
            glm::vec4 a, glm::vec4 b, glm::vec4 c, FragShaderBase& fs, const Barycentric& bary
        );

        void RenderBottom(
            const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, FragShaderBase& fs,
            const Barycentric& bary
        );

        void RenderTop(
            const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, FragShaderBase& fs,
            const Barycentric& bary
        );
    };

} // namespace mt

#endif

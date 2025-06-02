#ifndef MT_FRAMEBUFFER_HPP_
#define MT_FRAMEBUFFER_HPP_

#include "../barycentric.hpp"
#include "../mtdefs.hpp"
#include "buffer.hpp"
#include "ib.hpp"
#include "plane.hpp"

//
#include "../shader.hpp"

namespace mt {

    class Texture;
    using cubemap_texture_t = std::array<Texture, 6>;

    enum BufferType {
        BCOLOR = 0x0001,
        BDEPTH = 0x0010,
    };

    class Framebuffer {
      public:

        Framebuffer(u32 width, u32 height)
            : m_color(Buffer<u32>(width, height)), m_depth(Buffer<f32>(width, height)),
              m_width(width), m_height(height), m_pitch(width * sizeof(u32)) {
            m_clip_frustum = { new ClipPlaneNear(), new ClipPlaneLeft(), new ClipPlaneRight(),
                               new ClipPlaneUp(), new ClipPlaneDown() };
            // ,  new ClipPlaneFar()
        }

        ~Framebuffer() = default;

        void RenderElements(ElementBuffer& ebo, VertexShaderBase& vs, FragShaderBase& fs);

        void RenderLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 color);

        void render_cubemap_fullscreen(
            const glm::mat4& inv_proj, const glm::mat4& inv_mat, const cubemap_texture_t& cubemap
        );

        const Buffer<u32>& GetColorBuffer(void) const { return m_color; }

        const Buffer<f32>& GetDepthBuffer(void) const { return m_depth; }

        u32                GetPitch(void) const { return m_pitch; }

        void               Clear(u32 buffers) {
            if (buffers & BCOLOR) m_color.Memset(0xdeadbeef);
            if (buffers & BDEPTH) m_depth.Memset(0.f);
        }

        u32* GetData() { return const_cast<u32*>(m_color.GetData()); }

        bool wireframe      = false;
        bool cull_backfaces = true;

      private:

        frustum_t   m_clip_frustum;
        Buffer<u32> m_color;
        Buffer<f32> m_depth;
        u64         m_width;
        u64         m_height;
        u32         m_pitch;

      private:

        void RenderTriangle(
            glm::vec4 p0, glm::vec4 p1, glm::vec4 p2, FragShaderBase& frag, const Barycentric& bary
        );
    };

} // namespace mt

#endif

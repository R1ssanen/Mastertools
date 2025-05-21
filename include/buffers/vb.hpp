#ifndef MT_VB_HPP_
#define MT_VB_HPP_

#include <cstring>
#include <vector>

#include "../mtdefs.hpp"
#include "buffer.hpp"

namespace mt {

    class VertexBuffer : public Buffer<f32> {

      public:

        VertexBuffer(const f32* data, u64 count, u8 per_vertex)
            : Buffer<f32>(count), m_per_vertex(per_vertex) {

            std::memmove(m_mem.data(), data, m_bytes);
            m_vertex_stride = per_vertex * GetStride();
            m_transformed   = m_mem;
        }

        ~VertexBuffer() = default;

        std::vector<f32>& GetTransformed(void) { return m_transformed; }

        u32               GetVertexStride(void) const { return m_vertex_stride; }

        u8                GetPerVertex(void) const { return m_per_vertex; }

      private:

        std::vector<f32> m_transformed;
        u32              m_vertex_stride;
        u8               m_per_vertex;
    };

} // namespace mt

#endif

#ifndef MT_IB_HPP_
#define MT_IB_HPP_

#include <cstring>

#include "../mtdefs.hpp"
#include "buffer.hpp"
#include "vb.hpp"

namespace mt {

    class IndexBuffer : public Buffer<u32> {

      public:

        IndexBuffer(const u32* indices, u64 count) : Buffer<u32>(count) {
            std::memmove(m_mem.data(), indices, m_bytes);
        }
    };

} // namespace mt

#endif

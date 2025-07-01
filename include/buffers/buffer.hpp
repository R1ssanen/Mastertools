#ifndef MT_BUFFER_HPP_
#define MT_BUFFER_HPP_

#include <cstring>
#include <vector>

#include "../mtdefs.hpp"

namespace mt {

    template <typename T> class Buffer {
      public:

        Buffer(u64 count) : Buffer(count, 1) { }

        Buffer(u32 width, u32 height) : m_count(width * height), m_width(width), m_height(height) {
            m_mem   = std::vector<T>(m_count);
            m_bytes = m_count * GetStride();
        }

        T&           operator[](u64 index) { return m_mem[index]; }

        const T*     GetData(void) const { return m_mem.data(); }

        u32          GetWidth(void) const { return m_width; }

        u32          GetHeight(void) const { return m_height; }

        u64          GetCount(void) const { return m_count; }

        constexpr u8 GetStride(void) const { return sizeof(T); }

        void         Memset(T value) { std::fill(m_mem.begin(), m_mem.end(), value); }

        void         Swap(T* ptr) {
            T* data = m_mem.data();
            std::swap(data, ptr);
        }

      protected:

        std::vector<T> m_mem;
        u64            m_count;
        u32            m_width;
        u32            m_height;
        u64            m_bytes;
    };

} // namespace mt

#endif

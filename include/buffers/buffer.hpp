#ifndef MT_BUFFER_HPP_
#define MT_BUFFER_HPP_

#include <cstring>
#include <string>
#include <unordered_map>

#include "../mtdefs.hpp"

namespace mt {

    template <typename T> class Buffer {
      public:

        Buffer(u64 count) : Buffer(count, 1) { }

        Buffer(u64 width, u64 height)
            : m_width(width), m_height(height), m_count(width * height), m_stride(sizeof(T)) {
            m_data  = new T[m_count];
            m_bytes = m_count * m_stride;
        }

        ~Buffer() { delete[] m_data; }

        T&       operator[](u64 index) const { return m_data[index]; }

        const T* GetData(void) const { return m_data; }

        u64      GetWidth(void) const { return m_width; }

        u64      GetHeight(void) const { return m_height; }

        u64      GetCount(void) const { return m_count; }

        u8       GetStride(void) const { return m_stride; }

        void     Memset(u32 value) { std::memset(m_data, value, m_bytes); }

      protected:

        T*  m_data;
        u64 m_width;
        u64 m_height;
        u64 m_count;
        u64 m_bytes;
        u8  m_stride;
    };

} // namespace mt

#endif

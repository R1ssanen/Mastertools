#pragma once

#include "../mtpch.hpp"
#include "noncopyable.hpp"

namespace mt {

    class UUID : public NonCopyable {
      public:

        UUID Register();

        operator u64() { return m_ID; }

        ~UUID() = default;

      private:

        UUID();
        u64 m_ID;
    };

} // namespace mt

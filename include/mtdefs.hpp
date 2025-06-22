#ifndef MT_DEFS_HPP_
#define MT_DEFS_HPP_

#include <cstdint>
#include <memory>

namespace mt {

    using u8                            = std::uint8_t;
    using u32                           = std::uint32_t;
    using i32                           = std::int32_t;
    using u64                           = std::uint64_t;

    using f32                           = float;
    using f64                           = double;

    template <typename T> using Ref     = std::shared_ptr<T>;
    template <typename T> using WeakRef = std::weak_ptr<T>;
    template <typename T> using Owning  = std::unique_ptr<T>;

} // namespace mt

#endif

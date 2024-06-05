#include <immintrin.h>

#include "../mtpch.hpp"

namespace mt::accel {

    using i32x4 = __m128i;

    using f32x4 = __m128;
    using f64x2 = __m128d;

    using i32x8 = __m256i;

    using f32x8 = __m256;
    using f64x4 = __m256d;

} // namespace mt::accel

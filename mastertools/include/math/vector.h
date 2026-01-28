#ifndef MASTERTOOLS_VECTOR_H_
#define MASTERTOOLS_VECTOR_H_

#include <immintrin.h>
#include <math.h>

typedef __m128 mvec;
typedef mvec mvec2f;
typedef mvec mvec3f;
typedef mvec mvec4f;

static inline mvec2f mt_vec2(float x, float y)
{
    return _mm_setr_ps(x, y, 0.f, 0.f);
}

static inline mvec3f mt_vec3(float x, float y, float z)
{
    return _mm_setr_ps(x, y, z, 0.f);
}

static inline mvec4f mt_vec4(float x, float y, float z, float w)
{
    return _mm_setr_ps(x, y, z, w);
}

static inline mvec mt_vec_zero(void)
{
    return _mm_setzero_ps();
}

static inline mvec mt_vec_set(float f)
{
    return _mm_set1_ps(f);
}

static inline mvec4f mt_vec3_to_vec4(mvec3f v, float f)
{
    return _mm_insert_ps(v, _mm_set_ss(f), 0x30);
}

// returns element 0
static inline float mt_vec_to_float(mvec a)
{
    return _mm_cvtss_f32(a);
}

static inline mvec mt_vec_add(mvec a, mvec b)
{
    return _mm_add_ps(a, b);
}

static inline mvec mt_vec_sub(mvec a, mvec b)
{
    return _mm_sub_ps(a, b);
}

static inline mvec mt_vec_mul(mvec a, mvec b)
{
    return _mm_mul_ps(a, b);
}

static inline mvec mt_vec_div(mvec a, mvec b)
{
    return _mm_div_ps(a, b);
}

static inline float mt_vec2_dot(mvec2f a, mvec2f b)
{
    return _mm_cvtss_f32(_mm_dp_ps(a, b, 0x31));
}

static inline float mt_vec3_dot(mvec3f a, mvec3f b)
{
    return _mm_cvtss_f32(_mm_dp_ps(a, b, 0x71));
}

static inline float mt_vec4_dot(mvec4f a, mvec4f b)
{
    return _mm_cvtss_f32(_mm_dp_ps(a, b, 0xf1));
}

static inline mvec3f mt_vec3_cross(mvec3f a, mvec3f b)
{
    mvec3f shuff_a = _mm_shuffle_ps(a, a, 0xd2);
    mvec3f shuff_b = _mm_shuffle_ps(b, b, 0xc9);

    mvec3f product = _mm_mul_ps(shuff_a, shuff_b);

    shuff_a = _mm_shuffle_ps(a, a, 0xc9);
    shuff_b = _mm_shuffle_ps(b, b, 0xd2);

    return _mm_fmsub_ps(shuff_a, shuff_b, product);
}

static inline mvec mt_vec2_length_v(mvec a)
{
    mvec len2 = _mm_dp_ps(a, a, 0x3f);
    return _mm_sqrt_ps(len2);
}

static inline mvec mt_vec2_normalize(mvec a)
{
    return _mm_div_ps(a, mt_vec2_length_v(a));
}

static inline mvec mt_vec3_length_v(mvec a)
{
    mvec len2 = _mm_dp_ps(a, a, 0x7f);
    return _mm_sqrt_ps(len2);
}

static inline mvec mt_vec3_normalize(mvec a)
{
    return _mm_div_ps(a, mt_vec3_length_v(a));
}

static inline mvec mt_vec4_length_v(mvec a)
{
    mvec len2 = _mm_dp_ps(a, a, 0xff);
    return _mm_sqrt_ps(len2);
}

static inline mvec mt_vec4_normalize(mvec a)
{
    return _mm_div_ps(a, mt_vec4_length_v(a));
}

static inline mvec mt_vec_inv(mvec a)
{
    return _mm_rcp_ps(a);
}

static inline mvec mt_vec_fma(mvec a, mvec b, mvec c)
{
    return _mm_fmadd_ps(a, b, c);
}

static inline void mt_vec2_store(float *dest, mvec2f a)
{
    _mm_storel_pi((__m64 *)dest, a);
}

static inline void mt_vec3_store(float *dest, mvec3f a)
{
    _mm_maskstore_ps(dest, _mm_setr_epi32(-1, -1, -1, 0), a);
}

static inline void mt_vec4_store(float *dest, mvec4f a)
{
    _mm_storeu_ps(dest, a);
}

#endif
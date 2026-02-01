#if !defined(__GNUC__) && !defined(__clang__)
#error "mastertools: gcc or clang required."
#endif

#ifndef MASTERTOOLS_MAT4_H_
#define MASTERTOOLS_MAT4_H_

#include <immintrin.h>

#include "vector.h"

typedef union mmat4 mmat4;
union mmat4 {
    struct
    {
        mvec4 m0, m1, m2, m3;
    };
    struct
    {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    float m[16];
} __attribute__((aligned(16)));

static inline mmat4 mt_mat4_identity(void)
{
    return (mmat4){.m00 = 1.f, .m11 = 1.f, .m22 = 1.f, .m33 = 1.f};
}

static inline mmat4 mt_mat4_zero(void)
{
    return (mmat4){.m0 = mt_vec4_zero(), .m1 = mt_vec4_zero(), .m2 = mt_vec4_zero(), .m3 = mt_vec4_zero()};
}

static inline mvec4 mt_mat4_mul_vec4(mmat4 m, mvec4 v)
{
    return mt_vec4(mt_vec4_dot(m.m0, v), mt_vec4_dot(m.m1, v), mt_vec4_dot(m.m2, v), mt_vec4_dot(m.m3, v));
}

static inline mmat4 mt_mat4_mul(mmat4 m0, mmat4 m1)
{
    mmat4 result;
    result.m0.vec =
        _mm_fmadd_ps(_mm_shuffle_ps(m0.m0.vec, m0.m0.vec, 0x00), m1.m0.vec,
                     _mm_fmadd_ps(_mm_shuffle_ps(m0.m0.vec, m0.m0.vec, 0x55), m1.m1.vec,
                                  _mm_fmadd_ps(_mm_shuffle_ps(m0.m0.vec, m0.m0.vec, 0xaa), m1.m2.vec,
                                               _mm_mul_ps(_mm_shuffle_ps(m0.m0.vec, m0.m0.vec, 0xff), m1.m3.vec))));
    result.m1.vec =
        _mm_fmadd_ps(_mm_shuffle_ps(m0.m1.vec, m0.m1.vec, 0x00), m1.m0.vec,
                     _mm_fmadd_ps(_mm_shuffle_ps(m0.m1.vec, m0.m1.vec, 0x55), m1.m1.vec,
                                  _mm_fmadd_ps(_mm_shuffle_ps(m0.m1.vec, m0.m1.vec, 0xaa), m1.m2.vec,
                                               _mm_mul_ps(_mm_shuffle_ps(m0.m1.vec, m0.m1.vec, 0xff), m1.m3.vec))));
    result.m2.vec =
        _mm_fmadd_ps(_mm_shuffle_ps(m0.m2.vec, m0.m2.vec, 0x00), m1.m0.vec,
                     _mm_fmadd_ps(_mm_shuffle_ps(m0.m2.vec, m0.m2.vec, 0x55), m1.m1.vec,
                                  _mm_fmadd_ps(_mm_shuffle_ps(m0.m2.vec, m0.m2.vec, 0xaa), m1.m2.vec,
                                               _mm_mul_ps(_mm_shuffle_ps(m0.m2.vec, m0.m2.vec, 0xff), m1.m3.vec))));
    result.m3.vec =
        _mm_fmadd_ps(_mm_shuffle_ps(m0.m3.vec, m0.m3.vec, 0x00), m1.m0.vec,
                     _mm_fmadd_ps(_mm_shuffle_ps(m0.m3.vec, m0.m3.vec, 0x55), m1.m1.vec,
                                  _mm_fmadd_ps(_mm_shuffle_ps(m0.m3.vec, m0.m3.vec, 0xaa), m1.m2.vec,
                                               _mm_mul_ps(_mm_shuffle_ps(m0.m3.vec, m0.m3.vec, 0xff), m1.m3.vec))));

    return result;
}

#endif

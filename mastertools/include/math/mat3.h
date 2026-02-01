#if !defined(__GNUC__) && !defined(__clang__)
#error "mastertools: gcc or clang required."
#endif

#ifndef MASTERTOOLS_MAT3_H_
#define MASTERTOOLS_MAT3_H_

#include <immintrin.h>

#include "vector.h"

typedef union mmat3 mmat3;
union mmat3 {
    struct
    {
        mvec3 m0, m1, m2;
    };
    struct
    {
        float m00, m01, m02, _pad0;
        float m10, m11, m12, _pad1;
        float m20, m21, m22, _pad2;
    };
    float m[12];
} __attribute__((aligned(16)));

static inline mmat3 mt_mat3_identity(void)
{
    return (mmat3){.m00 = 1.f, .m11 = 1.f, .m22 = 1.f};
}

static inline mmat3 mt_mat3_zero(void)
{
    return (mmat3){.m0 = mt_vec3_zero(), .m1 = mt_vec3_zero(), .m2 = mt_vec3_zero()};
}

static inline mvec3 mt_mat3_mul_vec3(mmat3 m, mvec3 v)
{
    return mt_vec3(mt_vec3_dot(m.m0, v), mt_vec3_dot(m.m1, v), mt_vec3_dot(m.m2, v));
}

static inline mmat3 mt_mat3_mul(mmat3 m0, mmat3 m1)
{
    mmat3 result;
    result.m0.vec = _mm_fmadd_ps(_mm_shuffle_ps(m0.m0.vec, m0.m0.vec, 0x00), m1.m0.vec,
                                 _mm_fmadd_ps(_mm_shuffle_ps(m0.m0.vec, m0.m0.vec, 0x55), m1.m1.vec,
                                              _mm_mul_ps(_mm_shuffle_ps(m0.m0.vec, m0.m0.vec, 0xaa), m1.m2.vec)));
    result.m1.vec = _mm_fmadd_ps(_mm_shuffle_ps(m0.m1.vec, m0.m1.vec, 0x00), m1.m0.vec,
                                 _mm_fmadd_ps(_mm_shuffle_ps(m0.m1.vec, m0.m1.vec, 0x55), m1.m1.vec,
                                              _mm_mul_ps(_mm_shuffle_ps(m0.m1.vec, m0.m1.vec, 0xaa), m1.m2.vec)));
    result.m2.vec = _mm_fmadd_ps(_mm_shuffle_ps(m0.m2.vec, m0.m2.vec, 0x00), m1.m0.vec,
                                 _mm_fmadd_ps(_mm_shuffle_ps(m0.m2.vec, m0.m2.vec, 0x55), m1.m1.vec,
                                              _mm_mul_ps(_mm_shuffle_ps(m0.m2.vec, m0.m2.vec, 0xaa), m1.m2.vec)));

    return result;
}

#endif
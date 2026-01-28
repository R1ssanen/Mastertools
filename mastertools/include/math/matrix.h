#ifndef MASTERTOOLS_MATRIX_H_
#define MASTERTOOLS_MATRIX_H_

#include <immintrin.h>

#include "vector.h"

typedef struct mmat3f mmat3f;
struct mmat3f
{
    mvec4f r0, r1, r2;
};

typedef struct mmat4f mmat4f;
struct mmat4f
{
    mvec4f r0, r1, r2, r3;
};

static inline mmat3f mt_mat3_identity(void)
{
    return (mmat3f){.r0 = mt_vec3(1, 0, 0), .r1 = mt_vec3(0, 1, 0), .r2 = mt_vec3(0, 0, 1)};
}

static inline mmat3f mt_mat3_zero(void)
{
    return (mmat3f){.r0 = mt_vec_zero(), .r1 = mt_vec_zero(), .r2 = mt_vec_zero()};
}

static inline mmat4f mt_mat4_identity(void)
{
    return (mmat4f){
        .r0 = mt_vec4(1, 0, 0, 0), .r1 = mt_vec4(0, 1, 0, 0), .r2 = mt_vec4(0, 0, 1, 0), .r3 = mt_vec4(0, 0, 0, 1)};
}

static inline mmat4f mt_mat4_zero(void)
{
    return (mmat4f){.r0 = mt_vec_zero(), .r1 = mt_vec_zero(), .r2 = mt_vec_zero(), .r3 = mt_vec_zero()};
}

static inline mvec3f mt_mat3_mul_vec3(mmat3f m, mvec3f v)
{
    return mt_vec3(mt_vec3_dot(m.r0, v), mt_vec3_dot(m.r1, v), mt_vec3_dot(m.r2, v));
}

static inline mvec4f mt_mat4_mul_vec4(mmat4f m, mvec4f v)
{
    return mt_vec4(mt_vec4_dot(m.r0, v), mt_vec4_dot(m.r1, v), mt_vec4_dot(m.r2, v), mt_vec4_dot(m.r3, v));
}

static inline mmat3f mt_mat3_mul(mmat3f m0, mmat3f m1)
{
    mmat3f result;
    result.r0 = _mm_fmadd_ps(
        _mm_shuffle_ps(m0.r0, m0.r0, 0x00), m1.r0,
        _mm_fmadd_ps(_mm_shuffle_ps(m0.r0, m0.r0, 0x55), m1.r1, _mm_mul_ps(_mm_shuffle_ps(m0.r0, m0.r0, 0xaa), m1.r2)));
    result.r1 = _mm_fmadd_ps(
        _mm_shuffle_ps(m0.r1, m0.r1, 0x00), m1.r0,
        _mm_fmadd_ps(_mm_shuffle_ps(m0.r1, m0.r1, 0x55), m1.r1, _mm_mul_ps(_mm_shuffle_ps(m0.r1, m0.r1, 0xaa), m1.r2)));
    result.r2 = _mm_fmadd_ps(
        _mm_shuffle_ps(m0.r2, m0.r2, 0x00), m1.r0,
        _mm_fmadd_ps(_mm_shuffle_ps(m0.r2, m0.r2, 0x55), m1.r1, _mm_mul_ps(_mm_shuffle_ps(m0.r2, m0.r2, 0xaa), m1.r2)));

    return result;
}

static inline mmat4f mt_mat4_mul(mmat4f m0, mmat4f m1)
{
    mmat4f result;
    result.r0 = _mm_fmadd_ps(_mm_shuffle_ps(m0.r0, m0.r0, 0x00), m1.r0,
                             _mm_fmadd_ps(_mm_shuffle_ps(m0.r0, m0.r0, 0x55), m1.r1,
                                          _mm_fmadd_ps(_mm_shuffle_ps(m0.r0, m0.r0, 0xaa), m1.r2,
                                                       _mm_mul_ps(_mm_shuffle_ps(m0.r0, m0.r0, 0xff), m1.r3))));
    result.r1 = _mm_fmadd_ps(_mm_shuffle_ps(m0.r1, m0.r1, 0x00), m1.r0,
                             _mm_fmadd_ps(_mm_shuffle_ps(m0.r1, m0.r1, 0x55), m1.r1,
                                          _mm_fmadd_ps(_mm_shuffle_ps(m0.r1, m0.r1, 0xaa), m1.r2,
                                                       _mm_mul_ps(_mm_shuffle_ps(m0.r1, m0.r1, 0xff), m1.r3))));
    result.r2 = _mm_fmadd_ps(_mm_shuffle_ps(m0.r2, m0.r2, 0x00), m1.r0,
                             _mm_fmadd_ps(_mm_shuffle_ps(m0.r2, m0.r2, 0x55), m1.r1,
                                          _mm_fmadd_ps(_mm_shuffle_ps(m0.r2, m0.r2, 0xaa), m1.r2,
                                                       _mm_mul_ps(_mm_shuffle_ps(m0.r2, m0.r2, 0xff), m1.r3))));
    result.r3 = _mm_fmadd_ps(_mm_shuffle_ps(m0.r3, m0.r3, 0x00), m1.r0,
                             _mm_fmadd_ps(_mm_shuffle_ps(m0.r3, m0.r3, 0x55), m1.r1,
                                          _mm_fmadd_ps(_mm_shuffle_ps(m0.r3, m0.r3, 0xaa), m1.r2,
                                                       _mm_mul_ps(_mm_shuffle_ps(m0.r3, m0.r3, 0xff), m1.r3))));

    return result;
}

#endif
#if !defined(__GNUC__) && !defined(__clang__)
#error "mastertools: gcc or clang required."
#endif

#ifndef MASTERTOOLS_QUAT_H_
#define MASTERTOOLS_QUAT_H_

#include <math.h>

#include "mat4.h"
#include "vec3.h"
#include "vec4.h"

typedef mvec4 mquat;

static inline mquat mt_quat_pure(mvec3 v)
{
    return mt_vec3_to_vec4(v, 0.f);
}

static inline mquat mt_quat_identity(void)
{
    return mt_vec4(0.f, 0.f, 0.f, 1.f);
}

static inline mquat mt_quat_conj(mquat q)
{
    mvec3 v = mt_vec4_to_vec3(q);
    return mt_vec3_to_vec4(mt_vec3_negate(v), q.w);
}

static inline mquat mt_quat_normalize(mquat q)
{
    return mt_vec4_normalize(q);
}

static inline mquat mt_quat_axis_angle(mvec3 axis, float theta)
{
    mvec3 v = mt_vec3_mul(axis, mt_vec3_all(sinf(0.5f * theta)));
    float w = cosf(0.5f * theta);
    return mt_vec3_to_vec4(v, w);
}

static inline mquat mt_quat_mul(mquat p, mquat q)
{
    mvec3 pv = mt_vec4_to_vec3(p);
    mvec3 qv = mt_vec4_to_vec3(q);

    mvec3 v = mt_vec3_fma(mt_vec3_all(p.w), qv, mt_vec3_fma(mt_vec3_all(q.w), pv, mt_vec3_cross(pv, qv)));
    float w = p.w * q.w - mt_vec3_dot(pv, qv);
    return mt_vec3_to_vec4(v, w);
}

// vprime = 2 * dot(u, v) * qv + (2 * s * s - 1) * v + 2 * s * cross(qv, v);

static inline mvec3 mt_quat_rotate_vec3(mquat q, mvec3 v)
{
    mvec3 qv = mt_vec4_to_vec3(q);

    __m128 dot_2 = _mm_set1_ps(2.f * mt_vec3_dot(qv, v));
    __m128 w_2 = _mm_set1_ps(2.f * q.w);
    __m128 two_w_sq_minus_1 = _mm_fmsub_ps(w_2, _mm_set1_ps(q.w), _mm_set1_ps(1.f));

    return (mvec3){.vec = _mm_fmadd_ps(
                       dot_2, q.vec, _mm_fmadd_ps(two_w_sq_minus_1, v.vec, _mm_mul_ps(w_2, mt_vec3_cross(qv, v).vec)))};
}

#define MT_SHUFFLE_MASK(x, y, z, w) (((w) << 6u) | ((z) << 4u) | ((y) << 2u) | (x))

static inline mmat4 mt_quat_to_rotation(mquat q)
{
    __m128 v = q.vec;
    __m128 two = _mm_set1_ps(2.f);
    __m128 x = _mm_set1_ps(q.x);
    __m128 z = _mm_set1_ps(q.z);

    __m128 yyz = _mm_shuffle_ps(v, v, MT_SHUFFLE_MASK(1, 1, 2, 3));
    __m128 zzy = _mm_shuffle_ps(v, v, MT_SHUFFLE_MASK(2, 2, 1, 3));
    __m128 yxx = _mm_shuffle_ps(v, v, MT_SHUFFLE_MASK(1, 0, 0, 3));
    __m128 xyy = _mm_shuffle_ps(v, v, MT_SHUFFLE_MASK(0, 1, 1, 3));

    __m128 t0 = _mm_fmadd_ps(x, yyz, _mm_mul_ps(_mm_setr_ps(-q.w, q.w, -q.w, 0.f), zzy));
    t0 = _mm_mul_ps(two, t0);

    __m128 t1 = _mm_fmadd_ps(yxx, yxx, _mm_mul_ps(zzy, zzy));
    t1 = _mm_sub_ps(_mm_set1_ps(1.f), _mm_mul_ps(two, t1));

    __m128 t2 = _mm_fmadd_ps(z, xyy, _mm_mul_ps(_mm_setr_ps(q.w, q.w, -q.w, 0.f), yxx));
    t2 = _mm_mul_ps(two, t2);

    return (mmat4){.m00 = t1[0],
                   .m01 = t0[0],
                   .m02 = t2[0],
                   .m03 = 0.f,
                   .m10 = t0[1],
                   .m11 = t1[1],
                   .m12 = t2[1],
                   .m13 = 0.f,
                   .m20 = t0[2],
                   .m21 = t2[2],
                   .m22 = t1[2],
                   .m23 = 0.f,
                   .m30 = 0.f,
                   .m31 = 0.f,
                   .m32 = 0.f,
                   .m33 = 1.f};
}

// float yaw = atan2f(2 * (q.w * q.y + q.z * q.x), 1 - 2 * (q.y * q.y + q.z * q.z));
// float pitch = asinf(2 * (q.w * q.z - q.x * q.y));
// float roll = atan2f(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.z * q.z));

static inline mvec3 mt_quat_to_euler(mquat q)
{
    __m128 v = q.vec;

    __m128 yzx = _mm_shuffle_ps(v, v, MT_SHUFFLE_MASK(1, 2, 0, 3));
    __m128 zxy = _mm_shuffle_ps(v, v, MT_SHUFFLE_MASK(2, 0, 1, 3));
    __m128 zmxy_v = _mm_mul_ps(_mm_and_ps(zxy, _mm_setr_ps(0.f, -0.f, 0.f, 0.f)), v);

    __m128 fmadd = _mm_fmadd_ps(_mm_set1_ps(q.w), yzx, zmxy_v);
    __m128 lhs = _mm_mul_ps(_mm_set1_ps(2.f), fmadd);

    __m128 yzx_sq = _mm_mul_ps(yzx, yzx);
    __m128 yzxz_sq = _mm_shuffle_ps(yzx_sq, yzx_sq, MT_SHUFFLE_MASK(0, 1, 2, 1));

    __m128 hadd = _mm_hadd_ps(yzxz_sq, yzxz_sq);
    __m128 rhs = _mm_fmadd_ps(_mm_set1_ps(-2.f), hadd, _mm_set1_ps(1.f));

    return mt_vec3(atan2(lhs[2], rhs[1]), atan2f(lhs[0], rhs[0]), asinf(lhs[1]));
}

#endif
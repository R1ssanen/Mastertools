#ifndef MASTERTOOLS_QUAT_H_
#define MASTERTOOLS_QUAT_H_

#include <math.h>

#include "vector.h"

typedef struct mquat mquat;
struct mquat
{
    mvec3f v;
    float w;
};

static inline mquat mt_quat_vec3(mvec3f v)
{
    return (mquat){.v = v, .w = 1.f};
}

static inline mquat mt_quat_pure(mvec3f v)
{
    return (mquat){.v = v, .w = 0.f};
}

static inline mquat mt_quat_axis_angle(mvec3f axis, float theta)
{
    return (mquat){.v = mt_vec_mul(axis, mt_vec_set(sinf(0.5f * theta))), .w = cosf(0.5f * theta)};
}

static inline mquat mt_quat_identity(void)
{
    return (mquat){.v = mt_vec_zero(), .w = 1.f};
}

static inline mquat mt_quat_conj(mquat q)
{
    return (mquat){.v = mt_vec_mul(q.v, mt_vec_set(-1.f)), .w = q.w};
}

static inline mquat mt_quat_normalize(mquat q)
{
    mvec4f u = mt_vec4_normalize(mt_vec3_to_vec4(q.v, q.w));
    return (mquat){.v = u, .w = _mm_cvtss_f32(_mm_shuffle_ps(u, u, 0xff))};
}

static inline mquat mt_quat_mul(mquat p, mquat q)
{
    return (mquat){
        .v = mt_vec_fma(mt_vec_set(p.w), q.v, mt_vec_fma(mt_vec_set(q.w), p.v, mt_vec3_cross(p.v, q.v))),
        .w = p.w * q.w - mt_vec3_dot(p.v, q.v),
    };
}

// vprime = 2.0f * dot(u, v) * u
//        + (2 * s * s - 1) * v
//        + 2.0f * s * cross(u, v);
static inline mvec3f mt_quat_rotate_vec3(mquat q, mvec3f v)
{
    // return mt_quat_mul(q, mt_quat_mul(mt_quat_pure(v), mt_quat_conj(q))).v;

    const mvec3f two_uv_dot = mt_vec_set(2.f * mt_vec3_dot(q.v, v));
    const mvec3f two_qw = mt_vec_set(2.f * q.w);
    const mvec3f two_qw2_minus_1 = mt_vec_set(2.f * q.w * q.w - 1.f);

    return mt_vec_fma(two_uv_dot, q.v, mt_vec_fma(two_qw2_minus_1, v, mt_vec_mul(two_qw, mt_vec3_cross(q.v, v))));
}

static inline mmat4f mt_quat_to_rotation(mquat q)
{
    const mvec4f v2 = mt_vec_mul(q.v, q.v);
    mmat4f rotation;

    rotation.r0 = mt_vec4(1.f - 2.f * (v2[1] + v2[2]), 2.f * (q.v[0] * q.v[1] - q.w * q.v[2]),
                          2.f * (q.v[0] * q.v[2] + q.w * q.v[1]), 0.f);
    rotation.r1 = mt_vec4(2.f * (q.v[0] * q.v[1] + q.w * q.v[2]), 1.f - 2.f * (v2[0] + v2[2]),
                          2.f * (q.v[1] * q.v[2] + q.w * q.v[0]), 0.f);
    rotation.r2 = mt_vec4(2.f * (q.v[0] * q.v[2] - q.w * q.v[1]), 2.f * (q.v[1] * q.v[2] - q.w * q.v[0]),
                          1.f - 2.f * (v2[0] + v2[1]), 0.f);
    rotation.r3 = mt_vec4(0.f, 0.f, 0.f, 1.f);

    return rotation;
}

#endif
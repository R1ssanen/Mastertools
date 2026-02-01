#if !defined(__GNUC__) && !defined(__clang__)
#error "mastertools: gcc or clang required."
#endif

#ifndef MASTERTOOLS_VECTOR_H_
#define MASTERTOOLS_VECTOR_H_

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

static inline mvec2 mt_vec3_to_vec2(mvec3 a)
{
    return mt_vec2(a.x, a.y);
}

static inline mvec2 mt_vec4_to_vec2(mvec4 a)
{
    return mt_vec2(a.x, a.y);
}

static inline mvec3 mt_vec2_to_vec3(mvec2 a, float z)
{
    return mt_vec3(a.x, a.y, z);
}

static inline mvec3 mt_vec4_to_vec3(mvec4 a)
{
    return mt_vec3(a.x, a.y, a.z);
}

static inline mvec4 mt_vec2_to_vec4(mvec2 a, float z, float w)
{
    return mt_vec4(a.x, a.y, z, w);
}

static inline mvec4 mt_vec3_to_vec4(mvec3 a, float w)
{
    return mt_vec4(a.x, a.y, a.z, w);
}

#endif
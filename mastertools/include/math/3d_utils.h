#ifndef MASTERTOOLS_3D_UTILS_H_
#define MASTERTOOLS_3D_UTILS_H_

#include "matrix.h"
#include "vector.h"

#define MT_PI 3.14159265359f
#define MT_PI_PER_180 (MT_PI / 180.f)
#define MT_RADIANS(degrees) (degrees * MT_PI_PER_180)

static inline mmat4f mt_transform(mvec3f p)
{
    mmat4f trans;
    trans.r0 = mt_vec4(1.f, 0.f, 0.f, p[0]);
    trans.r1 = mt_vec4(0.f, 1.f, 0.f, p[1]);
    trans.r2 = mt_vec4(0.f, 0.f, 1.f, p[2]);
    trans.r3 = mt_vec4(0.f, 0.f, 0.f, 1.f);
    return trans;
}

static inline mmat4f mt_view(mvec3f eye, mvec3f forward)
{
    mvec3f right = mt_vec3_cross(forward, mt_vec3(0.f, 1.f, 0.f));
    mvec3f up = mt_vec3_cross(right, forward);

    mmat4f view;
    view.r0 = mt_vec3_to_vec4(right, -mt_vec3_dot(right, eye));
    view.r1 = mt_vec3_to_vec4(up, -mt_vec3_dot(up, eye));
    view.r2 = mt_vec3_to_vec4(mt_vec_mul(mt_vec_set(-1), forward), mt_vec3_dot(forward, eye));
    view.r3 = mt_vec4(0.f, 0.f, 0.f, 1.f);
    return view;
}

static inline mmat4f mt_project(float fovh, float aspect_ratio, float near, float far)
{
    float fov_scale = 1.f / tanf(0.5f * MT_RADIANS(fovh));

    mmat4f proj;
    proj.r0 = mt_vec4(fov_scale / aspect_ratio, 0.f, 0.f, 0.f);
    proj.r1 = mt_vec4(0.f, fov_scale, 0.f, 0.f);
    proj.r2 = mt_vec4(0.f, 0.f, far / (near - far), -(far * near) / (far - near));
    proj.r3 = mt_vec4(0.f, 0.f, -1.f, 0.f);
    return proj;
}

#endif
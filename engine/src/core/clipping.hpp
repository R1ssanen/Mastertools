#pragma once

#include "../math/plane.hpp"
#include "../mtpch.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

namespace mt {

    void ClipTriangle(Triangle& Tri, const math::Plane& Plane, triangle_vector_t& o_ClipTriangles);

    void FrustumClipTriangle(
        const Triangle& Tri, const math::Frustum& Frustum, triangle_vector_t& o_ClipTriangles
    );

} // namespace mt

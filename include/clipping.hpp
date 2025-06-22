#ifndef MT_CLIPPING_HPP_
#define MT_CLIPPING_HPP_

#include <deque>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "mtdefs.hpp"
#include "plane.hpp"

namespace mt {

    using triangle_t = std::array<glm::vec4, 3>;

    void clip_triangle_homo_plane(
        const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2,
        std::vector<triangle_t>& triangles, const Plane& plane
    );

    std::vector<triangle_t> clip_triangle_homo_frustum(
        const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2, const f32* attribs0,
        const f32* attribs1, const f32* attribs2, const frustum_t& frustum
    );

} // namespace mt

#endif

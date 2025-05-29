#ifndef MT_CLIPPING_HPP_
#define MT_CLIPPING_HPP_

#include <glm/vec4.hpp>

namespace mt {

    enum class AAClipPlane { LEFT, RIGHT, UP, DOWN, NEAR, FAR };

    bool intersect_line_aa_plane(
        const glm::vec4& p0, const glm::vec4& p1, AAClipPlane plane, glm::vec4& intersection
    );

} // namespace mt

#endif

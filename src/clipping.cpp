#include "clipping.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include "mtdefs.hpp"

namespace mt {

    void clip_triangle_homo_plane(
        const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2,
        std::vector<triangle_t>& triangles, const Plane& plane
    ) {
        glm::vec4 tmp;

        switch ((plane.inside(p2) << 2) | (plane.inside(p1) << 1) | plane.inside(p0)) {

        case 0b111: triangles.push_back({ p0, p1, p2 });
        case 0b000: return;

        case 0b001:
            triangles.push_back(
                { p0, plane.line_intersection(p0, p1), plane.line_intersection(p0, p2) }
            );
            return;

        case 0b010:
            triangles.push_back(
                { plane.line_intersection(p1, p0), p1, plane.line_intersection(p1, p2) }
            );
            return;

        case 0b100:
            triangles.push_back(
                { plane.line_intersection(p2, p0), plane.line_intersection(p2, p1), p2 }
            );
            return;

        case 0b110:
            tmp = plane.line_intersection(p1, p0);
            triangles.push_back({ tmp, p1, p2 });
            triangles.push_back({ tmp, p2, plane.line_intersection(p2, p0) });
            return;

        case 0b101:
            tmp = plane.line_intersection(p0, p1);
            triangles.push_back({ p0, tmp, p2 });
            triangles.push_back({ tmp, plane.line_intersection(p2, p1), p2 });
            return;

        case 0b011:
            tmp = plane.line_intersection(p0, p2);
            triangles.push_back({ p0, p1, tmp });
            triangles.push_back({ tmp, p1, plane.line_intersection(p1, p2) });
            return;

        default: return;
        }
    }

    std::vector<triangle_t> clip_triangle_homo_frustum(
        const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2, const f32* attribs0,
        const f32* attribs1, const f32* attribs2, const frustum_t& frustum
    ) {

        std::vector<triangle_t> clipped = {
            { p0, p1, p2 }
        };

        for (const Plane* plane : frustum) {
            std::vector<triangle_t> tmp;
            tmp.reserve(clipped.size() * 2);

            for (const auto& [a, b, c] : clipped) clip_triangle_homo_plane(a, b, c, tmp, *plane);

            clipped = std::move(tmp);
        }

        return clipped;
    }

} // namespace mt

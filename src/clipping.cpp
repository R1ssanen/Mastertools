#include "clipping.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include "mtdefs.hpp"

namespace {

    enum ClipLayout {
        ALL_OUT = 0, // 000
        ALL_IN  = 7, // 111

        A_IN    = 1, // 001
        B_IN    = 2, // 010
        C_IN    = 4, // 100

        A_OUT   = 6, // 110
        B_OUT   = 5, // 101
        C_OUT   = 3, // 011
    };

} // namespace

namespace mt {

    void clip_triangle_homo_plane(
        const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2,
        std::vector<triangle_t>& triangles, const Plane& plane
    ) {
        glm::vec4 tmp;

        switch ((plane.inside(p2) << 2) | (plane.inside(p1) << 1) | plane.inside(p0)) {

        case ClipLayout::ALL_IN: triangles.push_back({ p0, p1, p2 });
        case ClipLayout::ALL_OUT: return;

        case ClipLayout::A_IN:
            triangles.push_back(
                { p0, plane.line_intersection(p0, p1), plane.line_intersection(p0, p2) }
            );
            return;

        case ClipLayout::B_IN:
            triangles.push_back(
                { plane.line_intersection(p1, p0), p1, plane.line_intersection(p1, p2) }
            );
            return;

        case ClipLayout::C_IN:
            triangles.push_back(
                { plane.line_intersection(p2, p0), plane.line_intersection(p2, p1), p2 }
            );
            return;

        case ClipLayout::A_OUT:
            tmp = plane.line_intersection(p1, p0);
            triangles.push_back({ tmp, p1, p2 });
            triangles.push_back({ tmp, p2, plane.line_intersection(p2, p0) });
            return;

        case ClipLayout::B_OUT:
            tmp = plane.line_intersection(p0, p1);
            triangles.push_back({ p0, tmp, p2 });
            triangles.push_back({ tmp, plane.line_intersection(p2, p1), p2 });
            return;

        case ClipLayout::C_OUT:
            tmp = plane.line_intersection(p0, p2);
            triangles.push_back({ p0, p1, tmp });
            triangles.push_back({ tmp, p1, plane.line_intersection(p1, p2) });
            return;

        default: return;
        }
    }

    std::vector<triangle_t> clip_triangle_homo_frustum(
        const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2, const frustum_t& frustum
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

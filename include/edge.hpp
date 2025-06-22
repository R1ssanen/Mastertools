#ifndef MT_EDGE_HPP_
#define MT_EDGE_HPP_

#include <algorithm>
#include <unordered_set>
#include <vector>

#include "mtdefs.hpp"

namespace mt {

    struct Edge {

        Edge() = default;

        Edge(u32 id0, u32 id1) : id0(id0), id1(id1) {
            auto [min, max] = std::minmax(id0, id1);
            hash            = std::hash<uint32_t>()(min) ^ (std::hash<uint32_t>()(max) << 1);
        }

        bool operator==(const Edge& other) const { return hash == other.hash; }

        u32  hash;
        u32  id0, id1;
        bool valid = true;
    };

    enum class DrawMode {
        TRIANGLE,
        LINE,
    };

    std::vector<Edge>        form_edges_triangle(const u32*, u64);
    std::vector<Edge>        form_edges_line(const u32*, u64);

    inline std::vector<Edge> form_edges(const u32* indices, u64 index_count, DrawMode mode) {

        switch (mode) {
        case DrawMode::TRIANGLE: return form_edges_triangle(indices, index_count);
        case DrawMode::LINE: return form_edges_line(indices, index_count);
        default: return {};
        }
    }

} // namespace mt

namespace std {

    template <> struct hash<mt::Edge> {
        size_t operator()(const mt::Edge& obj) const { return obj.hash; }
    };

} // namespace std

namespace mt {

    inline std::vector<Edge> form_edges_triangle(const u32* indices, u64 index_count) {
        std::vector<Edge> edges;
        edges.reserve(index_count);

        for (u64 i = 0; i < index_count; i += 3) {
            edges.emplace_back(indices[i], indices[i + 1]);
            edges.emplace_back(indices[i + 1], indices[i + 2]);
            edges.emplace_back(indices[i + 2], indices[i]);
        }

        return edges;

        /*
                std::unordered_set<Edge> edges;
        for (u64 i = 0; i < index_count; i += 3) {
            edges.insert((Edge){ indices[i + 0], indices[i + 1] });
            edges.insert((Edge){ indices[i + 1], indices[i + 2] });
            edges.insert((Edge){ indices[i + 2], indices[i + 0] });
        }

        return std::vector(edges.begin(), edges.end());
        */
    }

    inline std::vector<Edge> form_edges_line(const u32* indices, u64 index_count) {
        std::vector<Edge> edges;
        edges.reserve(index_count);

        for (u64 i = 0; i < index_count - 1; i += 1) edges.emplace_back(indices[i], indices[i + 1]);

        return edges;
    }

} // namespace mt

#include <glm/vec4.hpp>

#include "plane.hpp"

namespace mt {

    enum class LineClip { AS_IS, NEW, NONE };

    inline LineClip
    clip_edge(const glm::vec4& p0, const glm::vec4& p1, glm::vec4& pc, const Plane& plane) {
        bool in_0 = plane.inside(p0);
        bool in_1 = plane.inside(p1);

        if (!in_0 && !in_1) return LineClip::NONE;
        if (in_0 && in_1) return LineClip::AS_IS;

        if (in_0) pc = plane.line_intersection(p0, p1);
        else
            pc = plane.line_intersection(p1, p0);

        return LineClip::NEW;
    }

} // namespace mt

#endif

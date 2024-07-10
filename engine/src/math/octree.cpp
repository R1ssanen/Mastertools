#include "octree.hpp"

#include "../mtpch.hpp"
#include "aabb.hpp"

namespace mt::math {

    OctreeNode::OctreeNode(const math::AABB& AABB, u32 Depth, u32 Level, OctreeNode* Parent)
        : m(_M{ .Nodes = {}, .AABB = AABB, .Parent = Parent, .Depth = Depth, .Level = Level }) {

        // Octree generation has reached its depth
        if (Level > Depth) { return; }

        const auto& [Min, Max]    = AABB.GetExtents();
        const glm::vec3 HalfSides = (Max - Min) * 0.5f;

        for (u64 z = 0; z < 2; ++z) {
            for (u64 y = 0; y < 2; ++y) {
                u64 Loc = (z * 4) + (y * 2);

                for (u64 x = 0; x < 2; ++x) {
                    const glm::vec3 NewMin = Min + glm::vec3(x, y, z) * HalfSides;
                    m.Nodes[Loc + x]       = std::make_unique<OctreeNode>(
                        math::AABB(NewMin, NewMin + HalfSides), Depth, Level + 1, this
                    );
                }
            }
        }
    }

} // namespace mt::math

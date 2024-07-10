#pragma once

#include "../mtpch.hpp"
#include "aabb.hpp"

namespace mt::math {

    class OctreeNode {

      public:

        OctreeNode*                              FindBase() const;

        const std::array<Unique<OctreeNode>, 8>& GetNodes() const { return m.Nodes; }

        const math::AABB&                        GetAABB() const { return m.AABB; }

        const OctreeNode*                        GetParent() const { return m.Parent; }

        const u32&                               GetDepth() const { return m.Depth; }

        const u32&                               Level() const { return m.Level; }

        OctreeNode(const math::AABB& AABB, u32 Depth, u32 Level, OctreeNode* Parent);

      private:

        struct _M {
            std::array<Unique<OctreeNode>, 8> Nodes;
            math::AABB                        AABB;
            OctreeNode*                       Parent;
            u32                               Depth, Level;
        } m;
    };

    class Octree {
      public:

        const Unique<OctreeNode>& GetRootNode() const { return m_RootNode; }

        Octree(const math::AABB& AABB, u32 Depth)
            : m_RootNode(std::make_unique<OctreeNode>(AABB, Depth, 1, nullptr)) { }

      private:

        Unique<OctreeNode> m_RootNode;
    };

    inline u64 ExpectedOctreeNodes(u32 Depth) { return (std::pow(8, (Depth + 1)) - 1) / 7 - 1; }

} // namespace mt::math

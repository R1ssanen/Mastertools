#include "intersection.hpp"

#include "../mtpch.hpp"

namespace {
    std::unordered_map<mt::math::VolumeIntersectionFlag, mt::u32> s_CullDebugColoring = {
        {mt::math::VolumeIntersectionFlag::IN,          0x00FF00FF},
        { mt::math::VolumeIntersectionFlag::OUT,        0xFF0000FF},
        { mt::math::VolumeIntersectionFlag::INTERSECTS, 0xFFFF00FF}
    };
}

namespace mt::math {

    Vertex IntersectLinePlane(const Plane& Plane, const Vertex& A, const Vertex& B) {
        const f32 T = glm::clamp(
            glm::dot(Plane.GetPoint() - glm::vec3(A.Pos), Plane.GetNormal()) /
                glm::dot(glm::vec3(B.Pos - A.Pos), Plane.GetNormal()),
            0.f, 1.f
        );

        return Vertex{ .Pos    = A.Pos + (B.Pos - A.Pos) * T,
                       .Normal = glm::normalize(A.Normal + (B.Normal - A.Normal) * T),
                       .UV     = A.UV + (B.UV - A.UV) * T,
                       .Light  = A.Light + (B.Light - A.Light) * T };
    }

    VolumeIntersectionFlag IntersectPlaneAABB(const math::AABB& AABB, const math::Plane& Plane) {
        const glm::vec3 BoxCenter         = (AABB.GetMax() + AABB.GetMin()) * 0.5f;
        const glm::vec3 PositiveExtents   = AABB.GetMax() - BoxCenter;

        f32             IntervalRadius    = glm::dot(PositiveExtents, glm::abs(Plane.GetNormal()));
        f32             DistanceFromPlane = Plane.SignedDistance(BoxCenter);

        // Intersection occurs when distance falls within [-Radius, +Radius] interval
        if (DistanceFromPlane < -IntervalRadius) {
            return VolumeIntersectionFlag::OUT;
        }

        else if (DistanceFromPlane > IntervalRadius) {
            return VolumeIntersectionFlag::IN;
        }

        else {
            return VolumeIntersectionFlag::INTERSECTS;
        }
    }

    u32 GetDebugCullColor(VolumeIntersectionFlag Flag) { return s_CullDebugColoring[Flag]; }

} // namespace mt::math

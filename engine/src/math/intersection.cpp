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

    u32    GetDebugCullColor(VolumeIntersectionFlag Flag) { return s_CullDebugColoring[Flag]; }

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

    std::optional<glm::vec3> IntersectRayTriangle(
        const math::Ray& Ray, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C
    ) {
        constexpr f32 Epsilon       = std::numeric_limits<f32>::epsilon();

        glm::vec3     Edge1         = B - A;
        glm::vec3     Edge2         = C - A;
        glm::vec3     RayCrossEdge2 = glm::cross(Ray.GetDirection(), Edge2);
        f32           Det           = glm::dot(Edge1, RayCrossEdge2);

        // This ray is parallel to this triangle.
        if (Det > -Epsilon && Det < Epsilon) { return std::nullopt; }

        f32       InvDet = 1.f / Det;
        glm::vec3 RayToA = Ray.GetOrigin() - A;
        f32       U      = InvDet * glm::dot(RayToA, RayCrossEdge2);

        if (U < 0 || U > 1) { return std::nullopt; }

        glm::vec3 RayToACrossEdge1 = glm::cross(RayToA, Edge1);
        f32       V                = InvDet * glm::dot(Ray.GetDirection(), RayToACrossEdge1);

        if (V < 0 || U + V > 1) { return std::nullopt; }

        // At this stage we can compute t to find out where the intersection point is on the
        // line.
        f32 T = InvDet * glm::dot(Edge2, RayToACrossEdge1);

        // ray intersection
        if (T > Epsilon) {
            return Ray.Trace(T);
        } else { // This means that there is a line intersection but not a ray intersection.
            return std::nullopt;
        }
    }

} // namespace mt::math

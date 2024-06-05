#pragma once

#include "../math/aabb.hpp"
#include "../math/intersection.hpp"
#include "../math/plane.hpp"
#include "../mtpch.hpp"
#include "camera.hpp"
#include "triangle.hpp"

namespace mt {

    inline b8 WorldspaceBackfaceCull(const Triangle& Tri, const Camera& Camera) {
        const auto [a, b, c] = Tri.m_Vertices;
        glm::vec3 Normal =
            Camera.GetMatView() *
            glm::vec4(
                glm::cross(glm::vec3(c.Normal - a.Normal), glm::vec3(b.Normal - a.Normal)), 1.f
            );

        glm::vec3 View = Camera.GetMatView() * glm::vec4(Camera.GetForward(), 1.f);

        //! Mesh->Texture->IsDoublesided() &&
        return glm::dot(glm::normalize(View), glm::normalize(Normal)) <= 0.f;
    }

    inline b8 ClipspaceBackfaceCull(const Triangle& Tri) {
        const auto [a, b, c] = Tri.m_Vertices;
        return (b.Pos.x - a.Pos.x) * (c.Pos.y - a.Pos.y) -
                   (c.Pos.x - a.Pos.x) * (b.Pos.y - a.Pos.y) <=
               0.f;
    }

    inline math::VolumeIntersectionFlag
    FrustumCullBoundingSphere(const Camera& Camera, mesh_t Mesh, const glm::vec3& ObjectPos) {
        glm::vec3 ViewspaceMassCenter = Camera.GetMatView() *
                                        glm::translate(glm::mat4(1.f), ObjectPos) *
                                        glm::vec4(Mesh->GetMassCenter(), 1.f);
        f32 BoundingRadius = Mesh->GetBoundingRadius();

        for (const math::Plane& Plane : Camera.GetFrustum()) {
            f32 DistFromPlane = Plane.SignedDistance(ViewspaceMassCenter);

            if (DistFromPlane <= BoundingRadius) {
                if (DistFromPlane <= -BoundingRadius) { return math::VolumeIntersectionFlag::OUT; }

                return math::VolumeIntersectionFlag::INTERSECTS;
            }
        }

        return math::VolumeIntersectionFlag::IN;
    }

    // TODO: Fix this shit
    inline math::VolumeIntersectionFlag
    FrustumCullAABB(const Camera& Camera, mesh_t Mesh, const glm::vec3& ObjectPos) {

        glm::mat4         MatCamera    = Camera.GetMatView(); // glm::inverse(Camera.GetMatView());
        const glm::mat4&  MatTransform = MatCamera * glm::translate(glm::mat4(1.f), ObjectPos);

        const math::AABB& AABB         = Mesh->GetAABB();
        const math::AABB  TransformedAABB = math::AABB(
            MatTransform * glm::vec4(AABB.GetMin(), 1.f),
            MatTransform * glm::vec4(AABB.GetMax(), 1.f)
        );

        for (const math::Plane& Plane : Camera.GetFrustum()) {

            math::Plane WorldspacePlane = math::Plane(
                MatCamera * glm::vec4(Plane.GetNormal(), 0.f),
                MatCamera * glm::vec4(Plane.GetPoint(), 1.f)
            );

            auto Flag = math::IntersectPlaneAABB(TransformedAABB, Plane);
            if (Flag != math::VolumeIntersectionFlag::IN) { return Flag; }
        }

        return math::VolumeIntersectionFlag::IN;
    }

} // namespace mt

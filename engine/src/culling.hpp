#pragma once

#include "camera.hpp"
#include "clipping.hpp"
#include "mtpch.hpp"
#include "triangle.hpp"

namespace core {
    enum class CullFlag { IN, OUT, CLIP };
}

namespace {
    std::unordered_map<core::CullFlag, core::u32> s_CullDebugColoring = {
        { core::CullFlag::IN,   0x00FF00FF },
        { core::CullFlag::OUT,  0xFF0000FF },
        { core::CullFlag::CLIP, 0xFFFF00FF }
    };
}

namespace core {

    inline b8 WorldspaceBackfaceCull(const Triangle& Tri, const Camera& Camera) {
        const auto [a, b, c] = Tri.m_Vertices;
        glm::vec3 Normal =
            Camera.GetMatView() *
            glm::vec4(
                glm::cross(glm::vec3(c.m_Normal - a.m_Normal), glm::vec3(b.m_Normal - a.m_Normal)),
                1.f
            );

        glm::vec3 View = Camera.GetMatView() * glm::vec4(Camera.GetForward(), 1.f);

        //! Mesh->Texture->IsDoublesided() &&
        return glm::dot(glm::normalize(View), glm::normalize(Normal)) <= 0.f;
    }

    inline b8 ClipspaceBackfaceCull(const Triangle& Tri) {
        const auto [a, b, c] = Tri.m_Vertices;
        return (b.m_Pos.x - a.m_Pos.x) * (c.m_Pos.y - a.m_Pos.y) -
                   (c.m_Pos.x - a.m_Pos.x) * (b.m_Pos.y - a.m_Pos.y) <=
               0.f;
    }

    inline CullFlag
    FrustumCull(const Camera& Camera, mesh_t Mesh, const glm::vec3& ObjectPos) {

        glm::mat4 MatCamera = glm::inverse(Camera.GetMatView());
        glm::vec3 WorldspaceMassCenter =
            glm::translate(glm::mat4(1.f), ObjectPos) * glm::vec4(Mesh->GetMassCenter(), 1.f);
        f32 BoundingRadius = Mesh->GetBoundingRadius();

        for (const Plane& Plane : Camera.GetFrustum()) {

            core::Plane WorldspacePlane = Plane::New(
                MatCamera * glm::vec4(Plane.GetNormal(), 0.f),
                MatCamera * glm::vec4(Plane.GetPoint(), 1.f)
            );
            f32 DistFromPlane = WorldspacePlane.SignedDistance(WorldspaceMassCenter);

            if (DistFromPlane < BoundingRadius) {
                if (DistFromPlane < -BoundingRadius) { return CullFlag::OUT; }

                return CullFlag::CLIP;
            }
        }

        return CullFlag::IN;
    }

    inline u32 GetDebugCullColor(CullFlag Flag) { return s_CullDebugColoring[Flag]; }

} // namespace core

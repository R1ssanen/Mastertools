#include "clipping.hpp"

#include "../math/intersection.hpp"
#include "../math/plane.hpp"
#include "../mtpch.hpp"
#include "../resource/texture.hpp"
#include "triangle.hpp"

namespace mt {

    void ClipTriangle(Triangle& Tri, const math::Plane& Plane, triangle_vector_t& o_ClipTriangles) {
        auto [a, b, c] = Tri.m_Vertices;
        Vertex Temp;

        switch (((Plane.SignedDistance(c.Pos) > 0) << 2) +
                ((Plane.SignedDistance(b.Pos) > 0) << 1) + (Plane.SignedDistance(a.Pos) > 0)) {
        case 0: return;

        case 7: o_ClipTriangles.push_back(std::move(Tri)); return;

        case 1:
            Tri.m_Vertices = { a, math::IntersectLinePlane(Plane, a, b),
                               math::IntersectLinePlane(Plane, a, c) };
            o_ClipTriangles.push_back(std::move(Tri));
            return;

        case 2:
            Tri.m_Vertices = { math::IntersectLinePlane(Plane, b, a), b,
                               math::IntersectLinePlane(Plane, b, c) };
            o_ClipTriangles.push_back(std::move(Tri));
            return;

        case 4:
            Tri.m_Vertices = { math::IntersectLinePlane(Plane, c, a),
                               math::IntersectLinePlane(Plane, c, b), c };
            o_ClipTriangles.push_back(std::move(Tri));
            return;

        case 3:
            Temp = math::IntersectLinePlane(Plane, a, c);
            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{ a, b, Temp }, Tri.m_Shader, Tri.m_TextureID, 1.f
            );

            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{ Temp, b, math::IntersectLinePlane(Plane, b, c) },
                Tri.m_Shader, Tri.m_TextureID, 1.f
            );
            return;

        case 5:
            Temp = math::IntersectLinePlane(Plane, a, b);
            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{ a, Temp, c }, Tri.m_Shader, Tri.m_TextureID, 1.f
            );

            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{ Temp, math::IntersectLinePlane(Plane, c, b), c },
                Tri.m_Shader, Tri.m_TextureID, 1.f
            );
            return;

        default: // 6
            Temp = math::IntersectLinePlane(Plane, b, a);
            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{ Temp, b, c }, Tri.m_Shader, Tri.m_TextureID, 1.f
            );

            o_ClipTriangles.emplace_back(
                std::array<Vertex, 3>{ math::IntersectLinePlane(Plane, c, a), Temp, c },
                Tri.m_Shader, Tri.m_TextureID, 1.f
            );
            return;
        }
    }

    void FrustumClipTriangle(
        const Triangle& Tri, const math::Frustum& Frustum, triangle_vector_t& o_ClipTriangles
    ) {
        triangle_vector_t ClipTriangles = { Tri };

        for (const math::Plane& Plane : Frustum) {
            triangle_vector_t Temp;

            for (Triangle& Tri : ClipTriangles) { ClipTriangle(Tri, Plane, Temp); }

            ClipTriangles = std::move(Temp);
        }

        o_ClipTriangles.insert(o_ClipTriangles.end(), ClipTriangles.begin(), ClipTriangles.end());
    }

} // namespace mt

#include "light.hpp"

#include "../mtpch.hpp"
#include "../resource/mesh.hpp"
#include "vertex.hpp"

namespace mt {

    PointLight PointLight::New(const glm::vec3& Pos, const glm::vec3& Color, f32 Intensity) {
        return PointLight(_M{ .Pos = Pos, .Color = Color, .Intensity = Intensity });
    }

    f32 PointLight::GetLighting(const Vertex& Vertex) const {
        const f32 Distance{ glm::fastDistance(glm::vec3(Vertex.Pos), m.Pos) };
        return m.Intensity / (Distance * Distance);
    }

} // namespace mt

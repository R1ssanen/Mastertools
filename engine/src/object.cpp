#include "object.hpp"

#include "mesh.hpp"
#include "srpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core
{

glm::mat4 Object::GetMatScale() const { return glm::scale(glm::mat4(1.f), m.Scale); }

glm::mat4 Object::GetMatRotation(float t_DeltaTime) const {
    return glm::orientate4(m.Angle + t_DeltaTime);
}

glm::mat4 Object::GetMatTranslation() const { return glm::translate(glm::mat4(1.f), m.Pos); }

Object Object::New(const mesh_vector_t& t_Meshes, const glm::vec3& t_Pos, const glm::vec3& t_Angle,
                   const glm::vec3& t_Scale)
{
    return Object(_M{.Meshes = t_Meshes, .Pos = t_Pos, .Angle = t_Angle, .Scale = t_Scale});
}

} // namespace core

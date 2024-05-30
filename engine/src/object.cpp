#include "object.hpp"

#include "mesh.hpp"
#include "mtpch.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace core
{

glm::mat4 Object::GetMatScale() const { return glm::scale(glm::mat4(1.f), m.Scale); }

glm::mat4 Object::GetMatRotation(const glm::vec3& RotatedPoint) const {
    return glm::orientate4(m.Angle) * glm::translate(glm::mat4(1.f), RotatedPoint);
}

glm::mat4 Object::GetMatTranslation() const { return glm::translate(glm::mat4(1.f), m.Pos); }

glm::mat4 Object::GetMatModel(const glm::vec3& RotatedPoint) const {
    glm::mat4 MatRotation = GetMatRotation(RotatedPoint);
    glm::mat4 MatScale = GetMatScale();
    glm::mat4 MatTranslation = GetMatTranslation();

    return MatTranslation * MatRotation * MatScale;
}

Object Object::New(const mesh_vector_t& Meshes, const glm::vec3& Pos, const glm::vec3& Angle,
                   const glm::vec3& Scale)
{
    return Object(_M{.Meshes = Meshes, .Pos = Pos, .Angle = Angle, .Scale = Scale});
}

} // namespace core

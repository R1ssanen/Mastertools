#pragma once

#include "mesh.hpp"
#include "srpch.hpp"

namespace core
{

class Object
{
  public:
    glm::mat4 GetMatScale() const;
    glm::mat4 GetMatRotation(float t_DeltaTime = 0.f) const;
    glm::mat4 GetMatTranslation() const;

    const mesh_vector_t& GetMeshes() const { return m.Meshes; }
    const glm::vec3& GetPos() const { return m.Pos; }
    const glm::vec3& GetAngle() const { return m.Angle; }
    const glm::vec3& GetScale() const { return m.Scale; }

    static Object New(const mesh_vector_t& t_Meshes, const glm::vec3& t_Pos, const glm::vec3& t_Angle,
                      const glm::vec3& t_Scale);

  private:
    struct _M
    {
        mesh_vector_t Meshes;
        glm::vec3 Pos;
        glm::vec3 Angle;
        glm::vec3 Scale;
    } m;

    Object(_M&& t_Data) : m{std::move(t_Data)} {}
};

using object_vector_t = std::vector<Object>;

} // namespace core

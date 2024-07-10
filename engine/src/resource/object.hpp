#pragma once

#include "../mtpch.hpp"
#include "mesh.hpp"

namespace mt {

    class Object {
      public:

        glm::mat4            GetMatScale() const;
        glm::mat4            GetMatRotation(const glm::vec3& RotatedPoint = glm::vec3(0.f)) const;
        glm::mat4            GetMatTranslation() const;
        glm::mat4            GetMatModel(const glm::vec3& RotatedPoint = glm::vec3(0.f)) const;

        const mesh_vector_t& GetMeshes() const { return m.Meshes; }

        const glm::vec3&     GetPos() const { return m.Pos; }

        const glm::vec3&     GetAngle() const { return m.Angle; }

        const glm::vec3&     GetScale() const { return m.Scale; }

        Object(
            const mesh_vector_t& Meshes, const glm::vec3& Pos, const glm::vec3& Angle,
            const glm::vec3& Scale
        );

      private:

        struct _M {
            mesh_vector_t Meshes;
            glm::vec3     Pos;
            glm::vec3     Angle;
            glm::vec3     Scale;
        } m;

        explicit Object(_M&& Data) : m{ std::move(Data) } { }
    };

    using object_vector_t = std::vector<Object>;

} // namespace mt

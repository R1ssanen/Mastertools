#pragma once

#include "../mtpch.hpp"
#include "../resource/mesh.hpp"

namespace mt {

    class Particle {
      public:

        void             Update(f64 DeltaTime);

        const glm::vec3& GetPos() const { return m.Pos; }

        const glm::vec3& GetAngle() const { return m.Angle; }

        const glm::vec3& GetScale() const { return m.Scale; }

        const glm::vec3& GetVelocity() const { return m.Velocity; }

        const u64&       GetLifetime() const { return m.Lifetime; }

        const u64&       GetStartLifetime() const { return m.StartLifetime; }

        const u64&       GetID() const { return m.ID; }

        b8               operator==(const Particle& Other) { return m.ID == Other.GetID(); }

        static Particle
        New(const glm::vec3& Pos, const glm::vec3& Angle, const glm::vec3& Scale,
            const glm::vec3& Velocity, u64 Lifetime);

      private:

        struct _M {
            glm::vec3 Pos, Angle, Scale;
            glm::vec3 Velocity;
            u64       Lifetime, StartLifetime, ID;
        } m;

        explicit Particle(_M&& Data) : m{ std::move(Data) } { }
    };

    class Emitter {
      public:

        virtual void                 Update(f64 DeltaTime);
        virtual void                 Emit() = 0;

        const std::vector<Particle>& GetParticles() const { return m_Particles; }

        mesh_t                       GetMesh() const { return m_Mesh; }

        const glm::vec3&             GetPos() const { return m_Pos; }

        virtual ~Emitter() = default;

        Emitter(mesh_t Mesh, const glm::vec3& Pos, u64 ParticleCap)
            : m_Mesh{ Mesh }, m_Pos{ Pos }, m_ParticleCap{ ParticleCap } { }

      protected:

        std::vector<Particle> m_Particles;
        mesh_t                m_Mesh;
        glm::vec3             m_Pos;
        u64                   m_ParticleCap;
    };

    using emitter_t = std::unique_ptr<Emitter>;

    class SphericalEmitter : public Emitter {
      public:

        SphericalEmitter(const glm::vec3& Pos, f32 Radius, mesh_t Mesh, u64 ParticleCap)
            : Emitter(Mesh, Pos, ParticleCap), m_Radius{ Radius } { }

        void             Emit() override;

        static emitter_t New(mesh_t Mesh, u64 ParticleCap, const glm::vec3& Pos, f32 Radius);

      private:

        glm::vec3 m_Pos;
        f32       m_Radius;
    };

    class DiskEmitter : public Emitter {
      public:

        DiskEmitter(const glm::vec3& Pos, f32 Radius, mesh_t Mesh, u64 ParticleCap)
            : Emitter(Mesh, Pos, ParticleCap), m_Radius{ Radius } { }

        void             Emit() override;

        static emitter_t New(mesh_t Mesh, u64 ParticleCap, const glm::vec3& Pos, f32 Radius);

      private:

        f32 m_Radius;
    };

} // namespace mt

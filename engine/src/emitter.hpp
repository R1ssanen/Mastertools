#pragma once

#include "srpch.hpp"
#include "mesh.hpp"

namespace core {

    class Particle {
        public:
            void Update(double t_DeltaTime);

            const glm::vec3& GetPos() const {return m.Pos;}
            const glm::vec3& GetAngle() const {return m.Angle;}
            const glm::vec3& GetScale() const {return m.Scale;}
            const glm::vec3& GetVelocity() const {return m.Velocity;}

            const size_t& GetLifetime() const {return m.Lifetime;}
            const size_t& GetStartLifetime() const {return m.StartLifetime;}
            const size_t& GetID() const {return m.ID;}

            bool operator == (const Particle& t_Other) {
                return m.ID == t_Other.GetID();
            }

            static Particle New(const glm::vec3& t_Pos,
                                const glm::vec3& t_Angle,
                                const glm::vec3& t_Scale,
                                const glm::vec3& t_Velocity,
                                size_t t_Lifetime);
        private:
            struct _M {
                glm::vec3 Pos, Angle, Scale;
                glm::vec3 Velocity;
                size_t Lifetime, StartLifetime, ID;
            } m;

            Particle(_M&& t_Data) : m{std::move(t_Data)} {}
    };

    class Emitter {
        public:
            virtual ~Emitter() = default;
            Emitter(mesh_t t_Mesh, size_t t_ParticleCap)
                : m_Mesh{t_Mesh}, m_ParticleCap{t_ParticleCap} {}

            const std::vector<Particle>& GetParticles() const {return m_Particles;}
            mesh_t GetMesh() const {return m_Mesh;}

            virtual void Update(double t_DeltaTime);
            virtual void Emit() = 0;

        protected:
            std::vector<Particle> m_Particles;
            mesh_t m_Mesh;
            size_t m_ParticleCap;
    };

    using emitter_t = std::unique_ptr<Emitter>;

    class SphericalEmitter : public Emitter {
        public:
            ~SphericalEmitter() = default;
            SphericalEmitter(const glm::vec3& t_Pos, float t_Radius, mesh_t t_Mesh, size_t t_ParticleCap)
                : Emitter(t_Mesh, t_ParticleCap), m_Pos{t_Pos}, m_Radius{t_Radius} {}

            void Emit() override;

            static emitter_t New(mesh_t t_Mesh, size_t t_ParticleCap, const glm::vec3& t_Pos, float t_Radius);

        private:
            glm::vec3 m_Pos;
            float m_Radius;
    };

    class DiskEmitter : public Emitter {
        public:
            ~DiskEmitter() = default;
            DiskEmitter(const glm::vec3& t_Pos, float t_Radius, mesh_t t_Mesh, size_t t_ParticleCap)
                : Emitter(t_Mesh, t_ParticleCap), m_Pos{t_Pos}, m_Radius{t_Radius} {}
            
            void Emit() override;

            static emitter_t New(mesh_t t_Mesh, size_t t_ParticleCap, const glm::vec3& t_Pos, float t_Radius);

        private:
            glm::vec3 m_Pos;
            float m_Radius;
    };

}

#include "emitter.hpp"

#include "srpch.hpp"
#include "mesh.hpp"

namespace {
    size_t s_MaxParticleID = 0;
}

namespace core {

    Particle Particle::New(const glm::vec3& t_Pos,
                           const glm::vec3& t_Angle,
                           const glm::vec3& t_Scale,
                           const glm::vec3& t_Velocity,
                           size_t t_Lifetime) {
        return Particle(
            _M{
                .Pos = t_Pos,
                .Angle = t_Angle,
                .Scale = t_Scale,
                .Velocity = t_Velocity,
                .Lifetime = t_Lifetime,
                .StartLifetime = t_Lifetime,
                .ID = s_MaxParticleID++
            }
        );
    }

    void Particle::Update(double t_DeltaTime) {
        m.Lifetime--;
        m.Pos += m.Velocity;
        m.Angle += 0.01f;
    }

    void Emitter::Update(double t_DeltaTime) {
        for (Particle& Particle : m_Particles) {
            Particle.Update(t_DeltaTime);

            if (Particle.GetLifetime() == 0) {
                auto it = std::find(m_Particles.begin(), m_Particles.end(), Particle);
                m_Particles.erase(it);
                Emit();
            }
        }
    }

    void SphericalEmitter::Emit() {
        Particle Particle = Particle::New(
            glm::sphericalRand(m_Radius),
            glm::sphericalRand(glm::pi<float>()),
            glm::vec3(4.f),
            glm::vec3(0.f, 0.001f, 0.f),
            glm::linearRand(500.f, 10000.f)
        );

        m_Particles.push_back(Particle);
    }

    emitter_t SphericalEmitter::New(mesh_t t_Mesh, size_t t_ParticleCap, const glm::vec3& t_Pos, float t_Radius) {

        emitter_t Emitter = std::make_unique<SphericalEmitter>(t_Pos, t_Radius, t_Mesh, t_ParticleCap);

        for (size_t i = 0; i < t_ParticleCap; i++) { Emitter->Emit(); }

        return Emitter;
    }

    void DiskEmitter::Emit() {
        glm::vec2 SpawnPlane = glm::diskRand(m_Radius);
    
        Particle Particle = Particle::New(
            glm::vec3(SpawnPlane.x + m_Pos.x, m_Pos.y, SpawnPlane.y + m_Pos.z),
            glm::sphericalRand(glm::pi<float>()),
            glm::vec3(5.f),
            glm::vec3(0.f, glm::linearRand(0.f, -0.004f), 0.f),
            glm::linearRand(50.f, 400.f)
        );

        m_Particles.push_back(Particle);
    }

    emitter_t DiskEmitter::New(mesh_t t_Mesh, size_t t_ParticleCap, const glm::vec3& t_Pos, float t_Radius) {

        emitter_t Emitter = std::make_unique<DiskEmitter>(t_Pos, t_Radius, t_Mesh, t_ParticleCap);

        for (size_t i = 0; i < t_ParticleCap; i++) { Emitter->Emit(); }

        return Emitter;
    }

}
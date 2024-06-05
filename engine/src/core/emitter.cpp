#include "emitter.hpp"

#include "../mtpch.hpp"
#include "../resource/mesh.hpp"

namespace {
    mt::u64 s_MaxParticleID = 0;
}

namespace mt {

    Particle Particle::New(
        const glm::vec3& Pos, const glm::vec3& Angle, const glm::vec3& Scale,
        const glm::vec3& Velocity, u64 Lifetime
    ) {
        return Particle(_M{ .Pos           = Pos,
                            .Angle         = Angle,
                            .Scale         = Scale,
                            .Velocity      = Velocity,
                            .Lifetime      = Lifetime,
                            .StartLifetime = Lifetime,
                            .ID            = s_MaxParticleID++ });
    }

    void Particle::Update(f64 DeltaTime) {
        m.Lifetime--;
        m.Pos += m.Velocity;
        // m.Angle.x += 0.01f;
    }

    void Emitter::Update(f64 DeltaTime) {

        for (Particle& Particle : m_Particles) {
            Particle.Update(DeltaTime);

            if (Particle.GetLifetime() == 0) {
                auto it = std::find(m_Particles.begin(), m_Particles.end(), Particle);
                m_Particles.erase(it);
                Emit();
            }
        }
    }

    void SphericalEmitter::Emit() {
        Particle Particle = Particle::New(
            glm::sphericalRand(m_Radius), glm::sphericalRand(glm::pi<f32>()), glm::vec3(4.f),
            glm::vec3(0.f, 0.001f, 0.f), glm::linearRand(500.f, 10000.f)
        );

        m_Particles.push_back(Particle);
    }

    emitter_t
    SphericalEmitter::New(mesh_t Mesh, u64 ParticleCap, const glm::vec3& Pos, f32 Radius) {

        emitter_t Emitter = std::make_unique<SphericalEmitter>(Pos, Radius, Mesh, ParticleCap);

        for (u64 i = 0; i < ParticleCap; ++i) { Emitter->Emit(); }

        return Emitter;
    }

    void DiskEmitter::Emit() {
        glm::vec2 SpawnPlane = glm::diskRand(m_Radius);

        Particle  Particle   = Particle::New(
            glm::vec3(SpawnPlane.x + m_Pos.x, m_Pos.y, SpawnPlane.y + m_Pos.z),
            glm::vec3(0.f), // glm::sphericalRand(glm::pi<f32>()),
            glm::vec3(0.3f), glm::vec3(0.f, glm::linearRand(0.f, -0.00015f), 0.f),
            glm::linearRand(150.f, 300.f)
        );

        m_Particles.push_back(std::move(Particle));
    }

    emitter_t DiskEmitter::New(mesh_t Mesh, u64 ParticleCap, const glm::vec3& Pos, f32 Radius) {

        emitter_t Emitter = std::make_unique<DiskEmitter>(Pos, Radius, Mesh, ParticleCap);

        for (u64 i = 0; i < ParticleCap; ++i) { Emitter->Emit(); }

        return Emitter;
    }

} // namespace mt

#pragma once

#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"
#include "skybox.hpp"
#include "mtpch.hpp"
#include "emitter.hpp"

namespace core
{

class Scene
{
  public:
    const object_vector_t& GetObjects() const { return m.Objects; }
    const std::vector<PointLight>& GetLights() const { return m.Lights; }
    const std::vector<emitter_t>& GetEmitters() const { return m.Emitters; }
    const std::optional<Skybox>& GetSkybox() const { return m.Skybox; }
    const Camera& GetCamera() const { return m.Camera; }
    const std::string& GetName() const { return m.Name; }

    void Update(f64 DeltaTime);

    static Scene New(const std::string& Path);

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&& Other) : m{std::move(Other.m)} {}

  private:
    struct _M
    {
        object_vector_t Objects;
        std::vector<emitter_t> Emitters;
        std::vector<PointLight> Lights;
  
        std::optional<core::Skybox> Skybox;
        core::Camera Camera;
  
        std::string Name;
    } m;

    Scene(_M&& Data) : m{std::move(Data)} {}
};

} // namespace core

#pragma once

#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"
#include "skybox.hpp"
#include "srpch.hpp"

namespace core
{

class Scene
{
  public:
    const object_vector_t& GetObjects() const { return m.Objects; }
    const std::vector<PointLight>& GetLights() const { return m.Lights; }
    const std::optional<Skybox>& GetSkybox() const { return m.Skybox; }
    const Camera& GetCamera() const { return m.Camera; }
    const std::string& GetName() const { return m.Name; }

    void Update(double t_DeltaTime);

    static Scene New(const std::string& t_Path);
    void operator=(const Scene& t_Other) { m = t_Other.m; }

  private:
    struct _M
    {
        object_vector_t Objects;
        std::vector<PointLight> Lights;
        std::optional<core::Skybox> Skybox;
        core::Camera Camera;
        std::string Name;
    } m;

    Scene(_M&& t_Data) : m{std::move(t_Data)} {}
};

} // namespace core

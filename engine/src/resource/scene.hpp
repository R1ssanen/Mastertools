#pragma once

#include "../core/camera.hpp"
#include "../core/emitter.hpp"
#include "../core/light.hpp"
#include "../core/skybox.hpp"
#include "../mtpch.hpp"
#include "object.hpp"

namespace mt {

    class Scene {
      public:

        const object_vector_t&         GetObjects() const { return m.Objects; }

        const std::vector<PointLight>& GetLights() const { return m.Lights; }

        const std::vector<emitter_t>&  GetEmitters() const { return m.Emitters; }

        const std::optional<Skybox>&   GetSkybox() const { return m.Skybox; }

        const Camera&                  GetCamera() const { return m.Camera; }

        const std::string&             GetName() const { return m.Name; }

        void                           Update(f64 DeltaTime);

        Scene(const std::string& Path);

        Scene(const Scene&)            = delete;
        Scene& operator=(const Scene&) = delete;

      private:

        struct _M {
            object_vector_t           Objects;
            std::vector<emitter_t>    Emitters;
            std::vector<PointLight>   Lights;

            std::optional<mt::Skybox> Skybox;
            mt::Camera                Camera = GetDefaultCamera();

            std::string               Name;
        } m;
    };

} // namespace mt

#include "scene.hpp"

#include "../core/emitter.hpp"
#include "../core/light.hpp"
#include "../core/skybox.hpp"
#include "../mtpch.hpp"
#include "object.hpp"

namespace {
    glm::vec3 JsonArrayToVec3(const nlohmann::json& Array) {
        return glm::vec3(Array[0].get<mt::f32>(), Array[1].get<mt::f32>(), Array[2].get<mt::f32>());
    }

    glm::vec4 JsonArrayToVec4(const nlohmann::json& Array) {
        return glm::vec4(
            Array[0].get<mt::f32>(), Array[1].get<mt::f32>(), Array[2].get<mt::f32>(),
            Array[3].get<mt::f32>()
        );
    }
} // namespace

namespace mt {

    void Scene::Update(f64 DeltaTime) {
        m.Camera.HandleMovement();
        m.Camera.HandleRotation();

        if (m.Skybox.has_value()) { m.Skybox.value().Update(); }

        for (emitter_t& Emitter : m.Emitters) { Emitter->Update(DeltaTime); }
    }

    Scene::Scene(const std::string& Path) {
        object_vector_t           Objects;
        std::vector<PointLight>   Lights;
        std::vector<emitter_t>    Emitters;
        std::optional<mt::Skybox> Skybox;
        Camera                    Camera = GetDefaultCamera();
        std::string               Name   = "Unnamed Map";

        using json                       = nlohmann::json;
        std::ifstream File(Path);
        if (!File) { throw std::runtime_error("Could not open level file '" + Path + "'."); }

        const json SceneData = json::parse(File);

        for (const auto& [Key, Element] : SceneData.items()) {
            if (Key == "scene") {
                if (!Element.get<std::string>().empty()) { Name = Element.get<std::string>(); }
            }

            else if (Key == "camera") {
                glm::vec3 CameraPos =
                    Element.contains("pos") ? JsonArrayToVec3(Element["pos"]) : Camera.GetPos();
                glm::vec3 CameraAngle = Element.contains("angle")
                                            ? JsonArrayToVec3(Element["angle"])
                                            : Camera.GetAngle();
                f32       CameraFov   = Element.contains("fov") ? Element["fov"].get<f32>()
                                                                : glm::degrees(Camera.GetFov());
                f32       CameraNear =
                    Element.contains("near") ? Element["near"].get<f32>() : Camera.GetNear();
                f32 CameraFar =
                    Element.contains("far") ? Element["far"].get<f32>() : Camera.GetFar();

                Camera = mt::Camera(CameraPos, CameraAngle, CameraFov, CameraNear, CameraFar);
            }

            else if (Key == "lights") {
                for (const auto& Light : Element["point"]) {
                    glm::vec3 LightPos =
                        Light.contains("pos") ? JsonArrayToVec3(Light["pos"]) : glm::vec3(0.f);
                    glm::vec3 LightColor =
                        Light.contains("color") ? JsonArrayToVec3(Light["color"]) : glm::vec3(1.f);
                    f32 LightIntensity =
                        Light.contains("intensity") ? Light["intensity"].get<f32>() : 1.f;

                    Lights.push_back(PointLight::New(LightPos, LightColor, LightIntensity));
                }
            }

            else if (Key == "skybox") {
                texture_t SkyboxTexture = Element.get<std::string>() == "default"
                                              ? GetDefaultSkyboxTexture()
                                              : ImageTexture::New(Element.get<std::string>());

                Skybox                  = mt::Skybox(SkyboxTexture->GetID());
            }

            else if (Key == "objects") {
                for (const auto& Object : Element) {
                    mesh_vector_t LoadedMeshes;

                    const b8      IsMipmapped =
                        Object.contains("mipmapped") ? Object["mipmapped"].get<b8>() : true;
                    const b8 IsDoublesided =
                        Object.contains("doublesided") ? Object["doublesided"].get<b8>() : false;
                    u8 Miplevels = Object.contains("miplevels") ? Object["miplevels"].get<u8>() : 4;
                    f32 AnimationPlaybackSpeed =
                        Object.contains("playback") ? Object["playback"].get<f32>() : 1.f;

                    if (Object.contains("texture") || Object.contains("color")) {
                        texture_t Texture = GetDefaultTexture();

                        if (Object.contains("texture")) {
                            const std::string TexturePath   = Object["texture"].get<std::string>();
                            const b8          IsTransparent = Object.contains("transparent")
                                                                  ? Object["transparent"].get<b8>()
                                                                  : false;

                            if (std::filesystem::path(TexturePath).extension() == ".gif") {
                                Texture = AnimatedTexture::New(
                                    TexturePath, AnimationPlaybackSpeed, IsTransparent,
                                    IsDoublesided
                                );
                            }

                            else {
                                Texture =
                                    IsMipmapped
                                        ? MipmapTexture::New(
                                              TexturePath, Miplevels, IsTransparent, IsDoublesided
                                          )
                                        : ImageTexture::New(
                                              TexturePath, IsTransparent, IsDoublesided
                                          );
                            }
                        }

                        else {
                            glm::vec4 Color         = JsonArrayToVec4(Object["color"]);
                            const b8  IsTransparent = Color.w != 1.f;
                            Texture                 = ColorTexture::New(
                                Path, ToUint32(Color), IsTransparent, IsDoublesided
                            );
                        }

                        LoadedMeshes.push_back(
                            LoadAsset(Object["path"].get<std::string>(), Texture->GetID())
                        );
                    }

                    else {
                        LoadedMeshes = LoadAsset(
                            Object["path"].get<std::filesystem::path>(), IsMipmapped, IsDoublesided
                        );
                    }

                    glm::vec3 ObjectPos =
                        Object.contains("pos") ? JsonArrayToVec3(Object["pos"]) : glm::vec3(0.f);
                    glm::vec3 ObjectAngle = Object.contains("angle")
                                                ? glm::radians(JsonArrayToVec3(Object["angle"]))
                                                : glm::vec3(0.f);
                    glm::vec3 ObjectScale = Object.contains("scale")
                                                ? JsonArrayToVec3(Object["scale"])
                                                : glm::vec3(1.f);

                    Objects.emplace_back(LoadedMeshes, ObjectPos, ObjectAngle, ObjectScale);
                }
            }

            else if (Key == "emitters") {
                for (const auto& Emitter : Element) {
                    glm::vec3 EmitterPos =
                        Emitter.contains("pos") ? JsonArrayToVec3(Emitter["pos"]) : glm::vec3(0.f);
                    f32 EmitterRadius =
                        Emitter.contains("radius") ? Emitter["radius"].get<f32>() : 0.25f;
                    u64 EmitterParticleCap =
                        Emitter.contains("particles") ? Emitter["particles"].get<u64>() : 10;

                    const b8 IsTransparent =
                        Emitter.contains("transparent") ? Emitter["transparent"].get<b8>() : false;
                    texture_t Texture = AnimatedTexture::New(
                        Emitter["texture"].get<std::string>(), 1.f, IsTransparent, false
                    );

                    mesh_t EmitterMesh =
                        LoadAsset(Emitter["path"].get<std::string>(), Texture->GetID());

                    if (Emitter["type"].get<std::string>() == "disk") {
                        Emitters.push_back(DiskEmitter::New(
                            EmitterMesh, EmitterParticleCap, EmitterPos, EmitterRadius
                        ));
                    }

                    else {
                        Emitters.push_back(SphericalEmitter::New(
                            EmitterMesh, EmitterParticleCap, EmitterPos, EmitterRadius
                        ));
                    }
                }
            }

            else [[unlikely]] {
                continue;
            }
        }

        File.close();

        m = _M{ .Objects  = Objects,
                .Emitters = std::move(Emitters),
                .Lights   = Lights,
                .Skybox   = Skybox,
                .Camera   = Camera,
                .Name     = Name };
    }

} // namespace mt

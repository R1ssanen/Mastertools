#include "scene.hpp"

#include "light.hpp"
#include "object.hpp"
#include "skybox.hpp"
#include "srpch.hpp"

namespace
{
glm::vec3 JsonArrayToVec3(const nlohmann::json& t_Array)
{
    return glm::vec3(t_Array[0].get<float>(), t_Array[1].get<float>(), t_Array[2].get<float>());
}
} // namespace

namespace core
{

void Scene::Update(double t_DeltaTime) {
    m.Camera.HandleMovement();
    m.Camera.HandleRotation();
    
    if (m.Skybox.has_value()) {
        m.Skybox.value().Update();
    }
}

Scene Scene::New(const std::string& t_Path)
{
    object_vector_t Objects;
    std::vector<PointLight> Lights;
    std::optional<Skybox> Skybox;
    Camera Camera = GetDefaultCamera();
    std::string Name = "Unnamed Map";

    using json = nlohmann::json;
    std::ifstream File(t_Path);
    const json SceneData{json::parse(File)};

    for (const auto& [Key, Element] : SceneData[0].items())
    {
        if (Key == "scene")
        {
            if (Element.get<std::string>().empty())
            {
                continue;
            }

            Name = Element.get<std::string>();
        }

        else if (Key == "camera")
        {
            glm::vec3 CameraPos = Element.contains("pos") ? JsonArrayToVec3(Element["pos"]) : Camera.GetPos();
            glm::vec3 CameraAngle = Element.contains("angle") ? JsonArrayToVec3(Element["angle"]) : Camera.GetAngle();
            float CameraFov = Element.contains("fov") ? Element["fov"].get<float>() : Camera.GetFov();
            float CameraNear = Element.contains("near") ? Element["near"].get<float>() : Camera.GetNear();
            float CameraFar = Element.contains("far") ? Element["far"].get<float>() : Camera.GetFar();

            Camera = Camera::New(CameraPos, CameraAngle, CameraFov, CameraNear, CameraFar);
        }

        else if (Key == "lights")
        {
            for (const auto& Light : Element["point"])
            {
                glm::vec3 LightPos = Light.contains("pos") ? JsonArrayToVec3(Light["pos"]) : glm::vec3(0.f);
                float LightIntensity = Light.contains("intensity") ? Light["intensity"].get<float>() : 1.f;

                Lights.push_back(PointLight::New(LightPos, LightIntensity));
            }
        }

        else if (Key == "skybox")
        {
            if (Element.contains("default") && Element["default"].get<bool>())
            {
                Skybox = GetDefaultSkybox();
            }

            else {
                Skybox = Skybox::New(LoadAsset(Element["directory"].get<std::string>(), Element["file"].get<std::string>(),
                                false, false)[0]);
            }
        }

        else if (Key == "objects")
        {
            for (const auto& Object : Element)
            {
                mesh_vector_t MeshesOut;

                for (const auto& Asset : Object["assets"])
                {
                    const mesh_vector_t LoadedMeshes{
                        LoadAsset(Asset["directory"].get<std::string>(), Asset["file"].get<std::string>(),
                                  Asset.contains("mipmapped") ? Asset["mipmapped"].get<bool>() : true,
                                  Asset.contains("doublesided") ? Asset["doublesided"].get<bool>() : false)};

                    MeshesOut.insert(MeshesOut.end(), LoadedMeshes.begin(), LoadedMeshes.end());
                }

                glm::vec3 ObjectPos = Object.contains("pos") ? JsonArrayToVec3(Object["pos"]) : glm::vec3(0.f);
                glm::vec3 ObjectAngle =
                    Object.contains("angle") ? glm::radians(JsonArrayToVec3(Object["angle"])) : glm::vec3(0.f);
                glm::vec3 ObjectScale = Object.contains("scale") ? JsonArrayToVec3(Object["scale"]) : glm::vec3(1.f);

                Objects.push_back(Object::New(MeshesOut, ObjectPos, ObjectAngle, ObjectScale));
            }
        }

        else [[unlikely]]
        {
            continue;
        }
    }

    File.close();

    return Scene(_M{.Objects = Objects, .Lights = Lights, .Skybox = Skybox, .Camera = Camera, .Name = Name});
}

} // namespace core
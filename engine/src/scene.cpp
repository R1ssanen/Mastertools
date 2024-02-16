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

glm::vec4 JsonArrayToVec4(const nlohmann::json& t_Array)
{
    return glm::vec4(t_Array[0].get<float>(), t_Array[1].get<float>(), t_Array[2].get<float>(), t_Array[3].get<float>());
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
                glm::vec3 LightColor = Light.contains("color") ? JsonArrayToVec3(Light["color"]) : glm::vec3(1.f);
                float LightIntensity = Light.contains("intensity") ? Light["intensity"].get<float>() : 1.f;

                Lights.push_back(PointLight::New(LightPos, LightColor, LightIntensity));
            }
        }

        else if (Key == "skybox")
        {
            texture_t SkyboxTexture = Element.get<std::string>() == "default" ?
                GetDefaultSkyboxTexture() : ImageTexture::New(Element.get<std::string>());

            Skybox = Skybox::New(SkyboxTexture->GetID());
        }

        else if (Key == "objects")
        {
            for (const auto& Object : Element)
            {
                mesh_vector_t LoadedMeshes;
                const bool IsMipmapped = Object.contains("mipmapped") ? Object["mipmapped"].get<bool>() : true;
                const bool IsDoublesided = Object.contains("doublesided") ? Object["doublesided"].get<bool>() : false;
                uint8_t Miplevels = Object.contains("miplevels") ? Object["miplevels"].get<uint8_t>() : 4;

                if (Object.contains("texture") || Object.contains("color")) {
                    texture_t Texture;

                    if (Object.contains("texture")) {
                        const bool IsTransparent = Object.contains("transparent") ? Object["transparent"].get<bool>() : false;
                        Texture = IsMipmapped ? MipmapTexture::New(Object["texture"].get<std::string>(), Miplevels, IsTransparent, IsDoublesided) : 
                                                ImageTexture::New(Object["texture"].get<std::string>(), IsTransparent, IsDoublesided);
                    }

                    else {
                        glm::vec4 Color = JsonArrayToVec4(Object["color"]) * 255.f;
                        const bool IsTransparent = Color.w != 1.f;
                        Texture = ColorTexture::New(t_Path, ToUint32(Color), IsTransparent, IsDoublesided);
                    }

                    LoadedMeshes.push_back(LoadAsset(Object["path"].get<std::string>(), Texture->GetID()));                                                       
                }

                else {
                    LoadedMeshes = LoadAsset(Object["path"].get<std::filesystem::path>(), IsMipmapped, IsDoublesided);
                }

                glm::vec3 ObjectPos = Object.contains("pos") ? JsonArrayToVec3(Object["pos"]) : glm::vec3(0.f);
                glm::vec3 ObjectAngle = Object.contains("angle") ? glm::radians(JsonArrayToVec3(Object["angle"])) : glm::vec3(0.f);
                glm::vec3 ObjectScale = Object.contains("scale") ? JsonArrayToVec3(Object["scale"]) : glm::vec3(1.f);

                Objects.push_back(Object::New(LoadedMeshes, ObjectPos, ObjectAngle, ObjectScale));
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
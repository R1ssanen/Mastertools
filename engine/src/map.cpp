#include "application.hpp"

#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "object.hpp"
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

void App::LoadMap(const std::string& t_Path)
{
    m_Objects.clear();
    m_PointLights.clear();

    using json = nlohmann::json;
    std::ifstream File(t_Path);
    const json MapData{json::parse(File)};

    for (const auto& [Key, Element] : MapData[0].items())
    {
        if (Key == "map")
        {
            m_MapName = Element.get<std::string>();

            if (m_MapName.empty()) [[unlikely]]
            {
                m_MapName = "Unnamed Map";
            }

            std::clog << "Loading map file '" + m_MapName + "' ...\n";
        }

        else if (Key == "camera")
        {
            m_Camera =
                Camera(Element.contains("pos") ? JsonArrayToVec3(Element["pos"]) : glm::vec3(0.f),
                       Element.contains("angle") ? glm::radians(JsonArrayToVec3(Element["angle"])) : glm::vec3(0.f),
                       Element.contains("fov") ? Element["fov"].get<float>() : 90.f,
                       Element.contains("near") ? Element["near"].get<float>() : 0.05f,
                       Element.contains("far") ? Element["far"].get<float>() : 100.f);
        }

        else if (Key == "lights")
        {
            for (const auto& Light : Element["point"])
            {
                PointLight PointLightOut(Light.contains("pos") ? JsonArrayToVec3(Light["pos"]) : glm::vec3(0.f),
                                         Light.contains("intensity") ? Light["intensity"].get<float>() : 1.f);
                m_PointLights.push_back(PointLightOut);
            }
        }

        else if (Key == "skybox")
        {
            m_Skybox = Skybox(LoadAsset(Element["directory"].get<std::string>(), Element["file"].get<std::string>(),
                                        false, false)[0]);
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

                core::Object ObjectOut(
                    MeshesOut, Object.contains("pos") ? JsonArrayToVec3(Object["pos"]) : glm::vec3(0.f),
                    Object.contains("angle") ? glm::radians(JsonArrayToVec3(Object["angle"])) : glm::vec3(0.f),
                    Object.contains("scale") ? JsonArrayToVec3(Object["scale"]) : glm::vec3(1.f));

                m_Objects.push_back(ObjectOut);
            }
        }

        else [[unlikely]]
        {
            continue;
        }
    }

    File.close();
    std::clog << "Map loading complete.\n";
}

} // namespace core

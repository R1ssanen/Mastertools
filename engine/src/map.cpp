#include "application.hpp"

#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "srpch.hpp"

namespace {

glm::vec3 JsonArrayToVec3(const nlohmann::json& t_Array) {
  return glm::vec3(t_Array[0].get<float>(), t_Array[1].get<float>(),
                   t_Array[2].get<float>());
}

}  // namespace

namespace core {

void Application::LoadMap(const std::string& t_Path) {
  m_Objects.clear();
  m_PointLights.clear();

  using json = nlohmann::json;
  std::ifstream File(t_Path);
  const json MapData{json::parse(File)};

  for (const auto& [Key, Element] : MapData[0].items()) {
    if (Key == "map") {
      m_MapName = Element.get<std::string>();

      if (m_MapName.empty()) [[unlikely]] {
        m_MapName = "Unnamed Map";
      }

      std::clog << "Loading map file '" + m_MapName + "' ...\n";
    }

    else if (Key == "camera") {
      auto v = Element["pos"];
      m_Camera =
          Camera(JsonArrayToVec3(Element["pos"]),
                 JsonArrayToVec3(Element["angle"]), Element["fov"].get<float>(),
                 Element["near"].get<float>(), Element["far"].get<float>());
    }

    else if (Key == "lights") {
      for (const auto& Light : Element["point"]) {
        m_PointLights.push_back(PointLight(JsonArrayToVec3(Light["pos"]),
                                           Light["intensity"].get<float>()));
      }

    }

    else if (Key == "objects") {
      for (const auto& Object : Element) {
        std::vector<Mesh> MeshesOut;

        for (const auto& Asset : Object["assets"]) {
          const std::vector<Mesh> LoadedMeshes{
              LoadMeshOBJ(Asset["directory"].get<std::string>(),
                          Asset["file"].get<std::string>())};

          MeshesOut.insert(MeshesOut.end(), LoadedMeshes.begin(),
                           LoadedMeshes.end());
        }

        core::Object ObjectOut(MeshesOut, JsonArrayToVec3(Object["pos"]),
                               JsonArrayToVec3(Object["angle"]),
                               JsonArrayToVec3(Object["scale"]));

        m_Objects.push_back(ObjectOut);
      };
    }

    else [[unlikely]] {
      continue;
    }
  }

  File.close();

  std::clog << "Map loading complete.\n";
}

}  // namespace core

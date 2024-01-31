#pragma once

#include "camera.hpp"
#include "context.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "skybox.hpp"
#include "srpch.hpp"
#include "timer.hpp"

namespace core {

class App {
 public:
  void Run();
  void Init();
  void Delete() {
    delete this;
  }
  void LogPerformance() {
    std::clog << "Avrg. Perf: " << m_Timer.Performance() << "ms/fr, "
              << m_Timer.Elapsed() << "ms Elapsed\n";
  }

  static App* GetInstance() {
    if (m_Instance == nullptr) {
      m_Instance = new App();
    }

    return m_Instance;
  }

  void SetRunning(const bool& t_Value) { m_Running = t_Value; }
  const bool& GetWireframe() const { return m_Wireframe; }
  void SetWireframe(const bool& t_Value) { m_Wireframe = t_Value; }
  const bool& GetShowMipmaps() const {return m_ShowMipmaps;}
  void SetShowMipmaps(const bool& t_Value) { m_ShowMipmaps = t_Value; }
  
  const Context& GetContext() const { return m_Context; }

  App(App& other) = delete;
  void operator=(const App&) = delete;

  void LoadMap(const std::string& t_Path = "../../engine/builtins/default.map");

 private:
  App() = default;
  ~App() = default;
  inline static App* m_Instance{nullptr};

  Skybox m_Skybox = Skybox(LoadMeshOBJ("../../engine/builtins/", "skybox.obj", false)[0]);
  Context m_Context = CreateContext();
  Timer m_Timer;

  std::string m_MapName;
  bool m_Running{true}, m_Wireframe{false}, m_ShowMipmaps{false};

  Camera m_Camera;

  std::vector<Object> m_Objects;
  std::vector<PointLight> m_PointLights;
};

}  // namespace core

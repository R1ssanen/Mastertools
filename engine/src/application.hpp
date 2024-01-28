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

class Application {
 public:
  void Run();
  void Init();
  void LogPerformance();

  static Application* GetInstance();

  void SetRunning(const bool& t_Value) { m_Running = t_Value; }
  void SetWireframe(const bool& t_Value) { m_Wireframe = t_Value; }
  const bool& GetWireframe() const { return m_Wireframe; }

  ~Application() = default;
  Application(Application& other) = delete;
  void operator=(const Application&) = delete;

  void LoadMap(const std::string& t_Path = "../../engine/builtins/default.map");

 private:
  Application() = default;

  inline static Application* m_Instance{nullptr};
  Context m_Context;
  Timer m_Timer;

  std::string m_MapName;
  bool m_Running{true}, m_Wireframe{false};

  Camera m_Camera;
  Skybox m_Skybox;

  std::vector<Object> m_Objects;
  std::vector<PointLight> m_PointLights;
};

}  // namespace core

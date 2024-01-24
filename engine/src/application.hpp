#pragma once

#include "camera.hpp"
#include "context.hpp"
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

  ~Application() = default;
  Application(Application& other) = delete;
  void operator=(const Application&) = delete;

 private:
  Application() = default;

  inline static Application* m_Instance{nullptr};
  bool m_Running{true}, m_Wireframe{false}, m_Paused{false};

  Context m_Context;
  std::vector<Object> m_Objects;
  Camera m_Camera;
  Timer m_Timer;
  Skybox m_Skybox;
};

}  // namespace core

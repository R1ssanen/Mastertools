#include "application.hpp"
#include "srpch.hpp"
#include "texture.hpp"

int main(int argc, char** argv) {
  std::clog << "Running SoftEngine 24.1.0 ...\n";

  auto app{core::Application::GetInstance()};

  try {
    app->Init();
    app->Run();

  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }

  app->LogPerformance();
  delete app;
  return 0;
}

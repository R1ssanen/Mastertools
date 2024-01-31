#include "application.hpp"
#include "srpch.hpp"
#include "texture.hpp"

int main(int argc, char** argv) {
  std::clog << "Running SoftEngine 24.1.0 ...\n";

  auto App{core::App::GetInstance()};

  try {
    App->Init();
    App->Run();

  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }

  App->LogPerformance();
  App->Delete();
  return 0;
}

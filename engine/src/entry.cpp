#include "application.hpp"
#include "mtpch.hpp"

core::i32 SDL_main(core::i32 argc, char** argv) {
    std::clog << "Running Mastertools Engine 24.2.0 ...\n";

    core::LoadSettings();
    auto App = core::App::GetInstance();

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

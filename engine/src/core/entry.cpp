#include "../mtpch.hpp"
#include "application.hpp"

mt::i32 main(mt::i32 argc, char** argv) {
    std::clog << "Running Mastertools Engine 24.3.0 ...\n";

    mt::LoadSettings();
    auto App = mt::App::GetInstance();

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

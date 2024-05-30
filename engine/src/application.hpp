#pragma once

#include "camera.hpp"
#include "context.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "scene.hpp"
#include "skybox.hpp"
#include "mtpch.hpp"
#include "timer.hpp"

namespace core {

    class App {
      public:

        void Run();
        void Init();

        void Delete() { delete this; }

        inline void LogPerformance() {
            std::clog << "Avrg. Perf: " << m_Timer.GetAveragePerformance() << "ms/fr, "
                      << m_Timer.GetTimeElapsed() << "ms Elapsed\n";
        }

        static App* GetInstance() {
            if (m_Instance == nullptr) { m_Instance = new App(); }

            return m_Instance;
        }

        void           SetRunning(const b8& Value) { m_Running = Value; }

        const Context& GetContext() const { return m_Context; }

        App(App& other)            = delete;
        void operator=(const App&) = delete;

      private:

        App()  = default;
        ~App() = default;

        inline static App* m_Instance = nullptr;
        b8               m_Running = true;
        Timer              m_Timer       = Timer::New();
        Context            m_Context     = Context::New();

        Scene              m_ActiveScene = Scene::New(BUILTINS_DIR + "fire_demo.mgls");
    };

} // namespace core

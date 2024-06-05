#pragma once

#include "../mtpch.hpp"
#include "../render/context.hpp"
#include "../resource/mesh.hpp"
#include "../resource/object.hpp"
#include "../resource/scene.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "skybox.hpp"
#include "timer.hpp"

namespace mt {

    class App {
      public:

        void        Run();
        void        Init();

        void        Delete() { delete this; }

        inline void LogPerformance() {

            std::clog << "Avrg perf. of " << m_Timer.GetTimeElapsed() / m_Timer.GetTicks()
                      << " ms / " << m_Timer.GetTicks() / m_Timer.GetTimeElapsed<seconds>()
                      << " fps.\n";

            std::clog << "Total Runtime of " << m_Timer.GetTimeElapsed<seconds>() << " s / "
                      << m_Timer.GetTicks() << " frames.\n";
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

        App()                         = default;
        ~App()                        = default;

        static inline App* m_Instance = nullptr;
        b8                 m_Running  = true;
        Timer              m_Timer;
        Context            m_Context     = Context::New();

        Scene              m_ActiveScene = Scene(BUILTINS_DIR + "default.mgls");
    };

} // namespace mt

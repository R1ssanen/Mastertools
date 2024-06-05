#include "timer.hpp"

#include "../mtpch.hpp"
#include "settings.hpp"

namespace mt {

    const u64& Timer::GetTicks() const noexcept { return m.Ticks; }

    void       Timer::Tick() noexcept {

        if (GetSettingMaxFPS() != 0) {
            timepoint_t BeforeSurplus    = std::chrono::high_resolution_clock::now();
            m.Frametime                  = BeforeSurplus - m.FrameStart;

            const u64   ElapsedNs        = m.Frametime.count();
            static u64  NsPerCappedFrame = 1000000000 / GetSettingMaxFPS();
            nanoseconds SurplusFrametime(0);

            if (ElapsedNs < NsPerCappedFrame) {
                SurplusFrametime = nanoseconds(NsPerCappedFrame - ElapsedNs);
                std::this_thread::sleep_for(SurplusFrametime);
            }

            timepoint_t AfterSurplus = BeforeSurplus + SurplusFrametime;
            m.FrameStart             = AfterSurplus;
            m.TimeElapsed += m.Frametime + SurplusFrametime;
            m.Ticks++;
        }

        else {
            timepoint_t Now = std::chrono::high_resolution_clock::now();
            m.Frametime     = Now - m.FrameStart;
            m.TimeElapsed += m.Frametime;
            m.Ticks++;
            m.FrameStart = Now;
        }
    }

    Timer::Timer() {
        timepoint_t StartTime = std::chrono::high_resolution_clock::now();

        m                     = _M{ .TimerStart  = StartTime,
                                    .FrameStart  = StartTime,
                                    .TimeElapsed = nanoseconds(0),
                                    .Frametime   = nanoseconds(0),
                                    .Ticks       = u64(0) };
    }

    Timer::Timer(const Timer& Other) : m(Other.m) { }

    Timer::Timer(Timer&& Other) : m(std::move(Other.m)) { }

} // namespace mt

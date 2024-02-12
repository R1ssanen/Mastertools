#include "timer.hpp"

#include "srpch.hpp"

namespace core
{

Timer Timer::New(double t_TimeElapsed, size_t t_FramesElapsed)
{
    return Timer(_M{.TimeFrameStart = std::chrono::high_resolution_clock::now(),
                    .TimeElapsed = t_TimeElapsed,
                    .DeltaTime = 0.0,
                    .Performance = 0.0,
                    .FramesElapsed = t_FramesElapsed});
}

void Timer::Tick()
{
    const auto TimeNow{std::chrono::high_resolution_clock::now()};

    m.DeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(TimeNow - m.TimeFrameStart).count();
    m.TimeFrameStart = TimeNow;
    m.TimeElapsed += m.DeltaTime;
    m.FramesElapsed++;
}

} // namespace core
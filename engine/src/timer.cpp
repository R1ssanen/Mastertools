#include "timer.hpp"

#include "mtpch.hpp"

namespace core
{

Timer Timer::New(f64 TimeElapsed, u64 FramesElapsed)
{
    return Timer(_M{.TimeFrameStart = std::chrono::high_resolution_clock::now(),
                    .TimeElapsed = TimeElapsed,
                    .DeltaTime = 0.0,
                    .Performance = 0.0,
                    .FramesElapsed = FramesElapsed});
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
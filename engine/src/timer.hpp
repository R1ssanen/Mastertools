#pragma once

#include "mtpch.hpp"

namespace core
{

class Timer
{
  public:
    const f64& GetTimeElapsed() const { return m.TimeElapsed; }
    const f64 GetAveragePerformance() const { return m.TimeElapsed / static_cast<f64>(m.FramesElapsed); }
    const u64& GetFramesElapsed() const { return m.FramesElapsed; }

    void Tick();

    static Timer New(f64 TimeElapsed = 0.f, u64 FramesElapsed = 1);

  private:
    struct _M
    {
        std::chrono::_V2::system_clock::time_point TimeFrameStart;
        f64 TimeElapsed, DeltaTime, Performance;
        u64 FramesElapsed;
    } m;

    Timer(_M&& Data) : m{std::move(Data)} {}
};

} // namespace core

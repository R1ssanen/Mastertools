#pragma once

#include "srpch.hpp"

namespace core
{

class Timer
{
  public:
    const double& GetTimeElapsed() const { return m.TimeElapsed; }
    const double GetAveragePerformance() const { return m.TimeElapsed / static_cast<double>(m.FramesElapsed); }
    const size_t& GetFramesElapsed() const { return m.FramesElapsed; }

    void Tick();

    static Timer New(double t_TimeElapsed = 0.f, size_t t_FramesElapsed = 1);

  private:
    struct _M
    {
        std::chrono::_V2::system_clock::time_point TimeFrameStart;
        double TimeElapsed, DeltaTime, Performance;
        size_t FramesElapsed;
    } m;

    Timer(_M&& t_Data) : m{std::move(t_Data)} {}
};

} // namespace core

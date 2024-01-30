#pragma once

#include "srpch.hpp"

namespace core {

class Timer {
 public:
  Timer() { m_TimeFrameStart = std::chrono::high_resolution_clock::now(); };
  ~Timer() = default;

  Timer(double t_TimeElapsed, size_t t_FramesElapsed)
      : m_TimeElapsed{t_TimeElapsed}, m_FramesElapsed{t_FramesElapsed} {
    m_TimeFrameStart = std::chrono::high_resolution_clock::now();
  }

  const double& Elapsed() const { return m_TimeElapsed; }

  const double Performance() const {
    return m_TimeElapsed / static_cast<double>(m_FramesElapsed);
  }

  const size_t& Frames() const { return m_FramesElapsed; }

  void Tick() {
    const auto TimeNow{std::chrono::high_resolution_clock::now()};

    m_DeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                      TimeNow - m_TimeFrameStart)
                      .count();
    m_TimeFrameStart = TimeNow;
    m_TimeElapsed += m_DeltaTime;
    m_FramesElapsed++;
  }

 private:
  double m_TimeElapsed{0.0}, m_DeltaTime{0.0}, m_Performance{0.0};
  size_t m_FramesElapsed{1};
  std::chrono::_V2::system_clock::time_point m_TimeFrameStart;
};

}  // namespace core

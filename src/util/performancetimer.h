#pragma once
#include <chrono>

#include "util/duration.h"
#include "util/highresolution_monotonic_clock.h"

class PerformanceTimer
{
public:
  // TODO: make this configurable via a template parameter?
  // note that std::chrono::steady_clock is not steady on all platforms
  using ClockT = HighResolutionMonotonicClock;
  PerformanceTimer()
          : m_startTime(){};

  void start() {
      m_startTime = ClockT::now();
  };

  mixxx::Duration elapsed() const {
      return mixxx::Duration::fromStdDuration(ClockT::now() - m_startTime);
  };
  mixxx::Duration restart() {
      const ClockT::time_point now = ClockT::now();
      const auto dur = mixxx::Duration::fromStdDuration(now - m_startTime);
      m_startTime = now;
      return dur;
  };

  mixxx::Duration difference(const PerformanceTimer& timer) const {
      return mixxx::Duration::fromStdDuration(m_startTime - timer.m_startTime);
  };

  bool running() const {
      return m_startTime.time_since_epoch().count() != 0;
  };

private:
  ClockT::time_point m_startTime;
};

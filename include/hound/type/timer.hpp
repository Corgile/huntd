//
// Created by brian on 11/23/23.
//
#ifndef HOUND_TIMER_HPP
#define HOUND_TIMER_HPP

#include <chrono>

namespace hd::type {
class [[nodiscard]] Timer {
public:
  Timer() = delete;

  Timer(double& elapsed) : m_elapsed(elapsed) {
    m_elapsed = 0;
    start_time = std::chrono::high_resolution_clock::now();
  }

  ~Timer() {
    elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
    /// pass out elapsed ms before dying
    m_elapsed = elapsed_time.count() * 1000.0f;
  }

private:
  double& m_elapsed;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  std::chrono::duration<double> elapsed_time = std::chrono::duration<double>::zero();
};

} // entity

#endif //HOUND_TIMER_HPP

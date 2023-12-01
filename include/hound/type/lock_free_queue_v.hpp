//
// Created by 裴沛东 on 2022/5/14.
//

#ifndef HOUND_LOCK_FREE_QUEUE_V
#define HOUND_LOCK_FREE_QUEUE_V

#include <atomic>
#include <array>
#include <optional>
#include <mutex>

#if defined(BENCHMARK)

  #include <hound/common/global.hpp>

#endif
namespace hd::entity {

template<typename T, size_t Capacity>
class LockFreeQueue {
public:
  LockFreeQueue() : buffer{}, head(0), tail(0) {}

  // 生产者 push
  void push(const T& value) {
    size_t currentTail = tail.load(std::memory_order_relaxed);
    size_t nextTail = (currentTail + 1) % Capacity;
    // 判断队列是否已满
    while (nextTail == head.load(std::memory_order_acquire)) {
      // 队列已满，等待
#if defined(BENCHMARK)
      global::num_dropped_packets++;
#endif
      std::this_thread::yield();
    }

    buffer[currentTail] = value;
    tail.store(nextTail, std::memory_order_release);
  }

  std::optional<T> pop() {
    // 判断队列是否为空
    while (head.load(std::memory_order_relaxed) == tail.load(std::memory_order_acquire)) {
      // 队列为空，等待
      std::this_thread::yield();
    }

    T value = buffer[head];
    head.store((head + 1) % Capacity, std::memory_order_release);
    return value;
  }

  // 返回队列中元素个数
  size_t count() const {
    std::scoped_lock<std::mutex> lock(mCounterMutex);
    return (tail.load(std::memory_order_acquire) - head.load(std::memory_order_acquire) + Capacity) % Capacity;
  }

  // 判断队列是否为空
  bool empty() const {
    std::scoped_lock<std::mutex> lock(mCounterMutex);
    return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
  }

private:
  std::array<T, Capacity> buffer;
  std::atomic<size_t> head;
  std::atomic<size_t> tail;
  mutable std::mutex mCounterMutex;
};

}// hd::entity
#endif //HOUND_LOCK_FREE_QUEUE_V
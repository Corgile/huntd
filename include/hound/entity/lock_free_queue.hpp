//
// Created by brian on 11/22/23.
//

#ifndef HOUND_LOCK_FREE_QUEUE_HPP
#define HOUND_LOCK_FREE_QUEUE_HPP

#include <atomic>
#include <array>

#if defined(BENCHMARK)

#include <hound/common/hd_global.hpp>

#endif

namespace hd::entity {
	template<typename T, std::size_t SIZE>
	class LockFreeQueue {
	private:
		std::array<T, SIZE> buffer_;
		std::atomic<std::size_t> head_;
		std::atomic<std::size_t> tail_;

	public:
		LockFreeQueue() : head_(0), tail_(0) {}

		bool push(const T& value) {
			std::size_t currentTail = tail_.load(std::memory_order_relaxed);
			std::size_t nextTail = (currentTail + 1) % SIZE;

			if (nextTail == head_.load(std::memory_order_acquire)) {
#if defined(BENCHMARK)
				global::num_missed_packet++;
#endif
				return false; // 队列已满
			}

			buffer_[currentTail] = value;
			tail_.store(nextTail, std::memory_order_release);
			return true;
		}

		bool pop(T& value) {
			std::size_t currentHead = head_.load(std::memory_order_relaxed);
			if (currentHead == tail_.load(std::memory_order_acquire)) {
				return false; // 队列为空
			}

			value = buffer_[currentHead];
			head_.store((currentHead + 1) % SIZE, std::memory_order_release);
#if defined(BENCHMARK)
			global::num_consumed_packet++;
#endif
			return true;
		}
	};

} // entity
// hd

#endif //HOUND_LOCK_FREE_QUEUE_HPP

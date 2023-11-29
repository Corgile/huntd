//
// Created by brian on 11/22/23.
//

#ifndef HOUND_LOCK_FREE_QUEUE_HPP
#define HOUND_LOCK_FREE_QUEUE_HPP

#include <atomic>
#include <array>

#if defined(BENCHMARK)

#include <hound/common/global.hpp>

#endif

namespace hd::type {
	template<typename T, int32_t SIZE>
	class LockFreeQueue {
	private:
		std::array<T, SIZE> mBuffer;
		std::atomic<std::size_t> mHead;
		std::atomic<std::size_t> mTail;
		std::atomic<int32_t> mCount;

	public:
		LockFreeQueue() : mHead(0), mTail(0), mCount(0) {}

		[[nodiscard("返回值不能忽略!")]] bool push(const T& value) {
			std::size_t currentTail = mTail.load(std::memory_order_relaxed);
			std::size_t nextTail = (currentTail + 1) % SIZE;

			if (nextTail == mHead.load(std::memory_order_acquire)) {
#if defined(BENCHMARK)
				global::num_missed_packet++;
#endif
				return false; // 队列已满
			}

			mBuffer[currentTail] = value;
			mTail.store(nextTail, std::memory_order_release);
			mCount.fetch_add(1);
			return true;
		}

		[[nodiscard("返回值不能忽略!")]] bool pop(T& value) {
			if (empty()) return false;
			std::size_t currentHead = mHead.load(std::memory_order_relaxed);
			value = mBuffer[currentHead];
			mHead.store((currentHead + 1) % SIZE, std::memory_order_release);
#if defined(BENCHMARK)
			global::num_consumed_packet++;
#endif
			mCount.fetch_sub(1);
			return true;
		}

		[[nodiscard("返回值不能忽略!")]]  bool empty() const {
			std::size_t currentHead = mHead.load(std::memory_order_relaxed);
			return currentHead == mTail.load(std::memory_order_acquire);
		}

		[[nodiscard("返回值不能忽略!")]] int32_t size() const {
			return mCount.load(std::memory_order_relaxed);
		}
	};

} // entity
// hd

#endif //HOUND_LOCK_FREE_QUEUE_HPP

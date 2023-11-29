//
// Created by brian on 11/22/23.
//

#ifndef HOUND_DOUBLE_BUFF_QUEUE_HPP
#define HOUND_DOUBLE_BUFF_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

namespace hd::entity {
	template<typename T>
	class DoubleBufferQueue {
	private:
		std::queue<T> buffer1_;
		std::queue<T> buffer2_;

		std::queue<T>* currentBuffer_;
		std::queue<T>* nextBuffer_;

		std::mutex mutex_;
		std::condition_variable cv_;

	public:
		DoubleBufferQueue() : currentBuffer_(&buffer1_), nextBuffer_(&buffer2_) {}

		void push(const T& item) {
			std::unique_lock<std::mutex> lock(mutex_);
			nextBuffer_->push(item);
			cv_.notify_one();
		}

		bool pop(T& item) {
			std::unique_lock<std::mutex> lock(mutex_);
			if (currentBuffer_->empty()) {
				cv_.wait(lock, [this]() { return !currentBuffer_->empty(); });
			}

			if (!currentBuffer_->empty()) {
				item = currentBuffer_->front();
				currentBuffer_->pop();
				return true;
			}
			return false;
		}

		void swapBuffers() {
			std::unique_lock<std::mutex> lock(mutex_);
			std::swap(currentBuffer_, nextBuffer_);
			cv_.notify_one();
		}
	};

} // entity

#endif //HOUND_DOUBLE_BUFF_QUEUE_HPP

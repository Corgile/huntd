//
// Created by brian on 11/23/23.
//
#if defined(BENCHMARK)
#ifndef HOUND_TIMER_HPP
#define HOUND_TIMER_HPP

#include <iostream>
#include <chrono>

namespace hd::entity {
	struct Timer {
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
		std::chrono::duration<float> duration{.0f};

		Timer() {
			start = std::chrono::high_resolution_clock::now();
		}

		~Timer() {
			duration = std::chrono::high_resolution_clock::now() - start;
			std::cout << "程序耗时 " << duration.count() << "s" << std::endl;
		}
	};

} // entity

#endif //HOUND_TIMER_HPP
#endif
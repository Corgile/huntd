//
// Created by brian on 11/22/23.
//

#ifndef HOUND_GLOBAL_HPP
#define HOUND_GLOBAL_HPP

#include <atomic>
#include <hound/type/capture_option.hpp>

namespace hd::global {
	extern hd::type::capture_option opt;
	extern std::string fillBit;
#if defined(BENCHMARK)
	extern uint32_t num_captured_packet;
	extern std::atomic<int32_t> num_missed_packet;
	extern std::atomic<int32_t> num_consumed_packet;
	extern std::atomic<int32_t> num_processed_packet;
#endif
}

#endif //HOUND_GLOBAL_HPP

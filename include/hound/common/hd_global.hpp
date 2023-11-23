//
// Created by brian on 11/22/23.
//

#ifndef HOUND_HD_GLOBAL_HPP
#define HOUND_HD_GLOBAL_HPP

#include <hound/entity/capture_option.hpp>

namespace hd::global {
	using namespace hd::entity;
	extern capture_option opt;
#if defined(BENCHMARK)
	extern uint32_t num_captured_packet;
	extern uint32_t num_missed_packet;
	extern uint32_t num_consumed_packet;
	extern uint32_t num_processed_packet;
#endif
}

#endif //HOUND_HD_GLOBAL_HPP
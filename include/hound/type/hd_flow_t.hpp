//
// Created by brian on 11/28/23.
//

#ifndef HOUND_HD_FLOW_T_HPP
#define HOUND_HD_FLOW_T_HPP

#include <string>
#include <vector>

namespace hd::entity {

	struct hd_packet_t {
		uint16_t ts_usec;
		uint32_t ts_sec;
		int32_t packet_len;
		std::string bitvec;
	};

	struct hd_flow_t {
		//! TODO 优化点
		std::string flowId;
		std::vector<hd_packet_t> data;
	};

} // entity

#endif //HOUND_HD_FLOW_T_HPP

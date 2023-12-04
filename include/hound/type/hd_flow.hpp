//
// Created by brian on 11/28/23.
//

#ifndef HOUND_HD_FLOW_T_HPP
#define HOUND_HD_FLOW_T_HPP

#include <string>
#include <vector>
#include <array>
#include <struct_json/json_writer.h>

namespace hd::entity {

struct hd_packet {
  __time_t ts_sec;
  __suseconds_t ts_usec;
  bpf_u_int32 packet_len;
  std::string bitvec;
};
REFLECTION(hd_packet, ts_usec, ts_sec, packet_len, bitvec)

struct hd_flow {
  //! TODO 优化点
  std::string flowId;
  int32_t count;
  std::vector<hd_packet> data;
};
REFLECTION(hd_flow, flowId, count, data)
} // entity

#endif //HOUND_HD_FLOW_T_HPP

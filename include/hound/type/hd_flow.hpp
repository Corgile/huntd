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
  uint32_t ts_sec;
  uint32_t ts_usec;
  uint32_t packet_len;
  std::string bitvec;
};
REFLECTION(hd_packet, ts_usec, ts_sec, packet_len, bitvec)

struct hd_flow {
  //! TODO 优化点
  std::string flowId;
  std::vector<hd_packet> data;
};
REFLECTION(hd_flow, flowId, data)
} // entity

#endif //HOUND_HD_FLOW_T_HPP
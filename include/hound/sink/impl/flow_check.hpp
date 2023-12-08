//
// Created by brian on 12/7/23.
//

#ifndef HOUND_FLOW_CHECK_HPP
#define HOUND_FLOW_CHECK_HPP

#include <hound/type/hd_flow.hpp>
#include <hound/common/global.hpp>
#include <hound/common/macro.hpp>
#include <hound/sink/impl/kafka/kafka_config.hpp>

namespace flow {
using namespace hd::entity;
using namespace hd::global;
using PacketList = std::vector<hd_packet>;

static void LoadKafkaConfig(kafka_config& config, std::string const& fileName) {
  std::ifstream config_file(fileName);
  if (not config_file.good()) {
    hd_line(RED("无法打开配置文件: "), fileName);
    exit(EXIT_FAILURE);
  }
  std::string line;
  while (std::getline(config_file, line)) {
    size_t pos{line.find('=')};
    if (pos == std::string::npos or line.at(0) == '#') continue;
    auto value{line.substr(pos + 1)};
    if (value.empty()) continue;
    auto key{line.substr(0, pos)};
    config.put(key, value);
    hd_line(BLUE("加载配置: "), key, "=", value);
  }
}

static bool IsFlowReady(PacketList const& existing, hd_packet const& newPacket) {
  if (existing.size() < opt.min_packets) return false;
  return existing.size() == opt.max_packets or
    existing.back().ts_sec - newPacket.ts_sec >= opt.packetTimeout;
}
}
#endif //HOUND_FLOW_CHECK_HPP

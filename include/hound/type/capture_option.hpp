//
// Created by brian on 11/22/23.
//

#ifndef HOUND_CAPTURE_OPTION_HPP
#define HOUND_CAPTURE_OPTION_HPP

#include <cstdint>
#include <unordered_map>
#include <string>

namespace hd::type {
struct capture_option {
  explicit capture_option();

  /// proto filter
  bool include_radiotap;
  bool include_wlan;
  bool include_eth;
  bool include_ip4;
  bool include_ipv6;
  bool include_tcp;
  bool include_udp;
  bool include_icmp;
  bool include_vlan;
  /// config
  int32_t payload_len;
  int32_t duration;
  int32_t output_index;
  int32_t num_packets;
  int32_t min_packets;
  int32_t max_packets;
  int32_t packetTimeout;
  int32_t stride;
  int32_t workers{1};
  std::string device;
  std::string filter;
  /// mode
  bool write_file;
  std::string output_file;
  bool send_kafka;
  std::string kafka_config;
  bool offline_mode;
  bool live_mode;
  std::string pcap_file;
  bool timestamp;
  bool verbose;
  bool unsign;
  bool caplen;
  int32_t fill_bit;
  /// output_index map
  std::unordered_map<int32_t, std::string> index_map;
public:
  void print();

  virtual ~capture_option();
};
}

#endif //HOUND_CAPTURE_OPTION_HPP

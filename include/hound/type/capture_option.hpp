//
// Created by brian on 11/22/23.
//

#ifndef HOUND_CAPTURE_OPTION_HPP
#define HOUND_CAPTURE_OPTION_HPP

#include <cstdint>
#include <string>

namespace hd::type {
struct capture_option final {
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
  int32_t payload;
  int32_t num_packets;
  int32_t packetTimeout;
  bool timestamp;
  bool verbose;
  bool unsign;
  bool caplen;
  int32_t fill_bit;

  int32_t stride;
  int32_t workers{1};
  std::string device;
  std::string filter;

#if defined(INCLUDE_KAFKA) || defined(LIVE_MODE)
  int32_t duration;
#endif

#ifdef INCLUDE_KAFKA
  bool send_kafka;
  std::string kafka_config;
  int32_t min_packets;
  int32_t max_packets;
#endif

#ifdef DEAD_MODE
  /// mode
  bool write_file;
  std::string output_file;
  bool offline_mode;
  std::string pcap_file;
#endif

#ifdef LIVE_MODE
  bool live_mode;
#endif

public:
  void print() const;

  ~capture_option();
};
}

#endif //HOUND_CAPTURE_OPTION_HPP

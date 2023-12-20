//
// Created by brian on 11/22/23.
//

#ifndef HOUND_CAPTURE_OPTION_HPP
#define HOUND_CAPTURE_OPTION_HPP

#include <cstdint>
#include <string>

namespace hd::type {
struct capture_option final {
  explicit capture_option() = default;

  /// proto filter
  bool include_radiotap{false};
  bool include_wlan{false};
  bool include_eth{false};
  bool include_ip4{true};
  bool include_ipv6{false};
  bool include_tcp{true};
  bool include_udp{true};
  bool include_icmp{false};
  bool include_vlan{false};
  /// config
  int32_t payload{20};
  int32_t num_packets{-1};
  int32_t packetTimeout{20};
  bool timestamp{false};
  bool verbose{false};
  bool unsign{true};
  bool caplen{true};
  int32_t fill_bit{0};

  int32_t stride{8};
  int32_t workers{1};
  std::string device{};
  std::string filter{};
  std::string output_file{};

#if defined(SEND_KAFKA) || defined(LIVE_MODE)
  int32_t duration{10};
#endif

#ifdef SEND_KAFKA
  bool send_kafka{false};
  std::string kafka_config{};
  int32_t min_packets{10};
  int32_t max_packets{100};
#endif

#ifdef DEAD_MODE
  /// mode
  bool write_file{false};
  bool offline_mode{false};
  std::string pcap_file{};
#endif

#ifdef LIVE_MODE
  bool live_mode{true};
#endif

public:
  void print() const;

  ~capture_option();
};
}

#endif //HOUND_CAPTURE_OPTION_HPP

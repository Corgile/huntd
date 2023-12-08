//
// Created by brian on 11/22/23.
//

#include <hound/common/macro.hpp>
#include <hound/type/capture_option.hpp>


hd::type::capture_option::capture_option() {
  /// filter
  this->include_radiotap = false;
  this->include_wlan = false;
  this->include_eth = false;
  this->include_ip4 = false;
  this->include_ipv6 = false;
  this->include_tcp = false;
  this->include_udp = false;
  this->include_icmp = false;
  this->include_vlan = false;
  this->timestamp = false;
  this->verbose = false;
  this->unsign = false;
  this->caplen = false;
  /// mode

  this->write_file = false;
  this->offline_mode = false;
  this->live_mode = true;
  /// config
  this->payload = 0;
  this->num_packets = -1;
  this->fill_bit = 0;
#ifdef INCLUDE_KAFKA
  this->send_kafka = false;
  this->min_packets = 5;
  this->max_packets = 100;
#endif
  this->packetTimeout = 20;
  this->duration = 0;
  this->stride = 8;
  this->kafka_config = {};
  this->output_file = {};
  this->filter = {};
  this->device = {};
}

// @formatter:off
void hd::type::capture_option::print() const {
  hd_info(CYAN("\n包含流量包的: "));
  if (include_radiotap) hd_info("radiotap, ");
  if (include_wlan) hd_info("wlan, ");
  if (include_eth) hd_info("eth, ");
  if (include_ip4) hd_info("ipv4, ");
  if (include_ipv6) hd_info("ipv6, ");
  if (include_tcp) hd_info("tcp, ");
  if (include_udp) hd_info("udp, ");
  if (include_icmp) hd_info("icmp, ");
  if (include_vlan) hd_info("vlan, ");
  if (payload > 0) hd_info("头部, ", payload, " 字节payload\n");
  if (caplen) hd_info(CYAN("报文长度, "));
  if (timestamp) hd_info(CYAN("时间戳"));

  if (num_packets > 0) {
    hd_line(CYAN("抓包个数: "), num_packets);
  }
  hd_line(CYAN("填充值: "), fill_bit);
  hd_line(CYAN("流超时时间: "), packetTimeout, "秒");
  hd_info(CYAN("将每 "), stride, CYAN(" 位一组"));
  if (unsign) hd_line(CYAN("按" YELLOW("无") CYAN("符号类型转换为10进制")));
  else hd_line(CYAN("按" YELLOW("有") CYAN("符号类型转换为10进制")));
  hd_line(CYAN("包处理线程: "), workers);
  hd_line(CYAN("filter: "), filter);
  //@formatter:on

#ifdef INCLUDE_KAFKA
  if (send_kafka) {
    if (not kafka_config.empty()) {
      hd_line(CYAN("kafka设置:  "), kafka_config);
    }
    hd_line(CYAN("流最小包数: "), min_packets);
    hd_line(CYAN("流最大包数: "), max_packets);
  }
#endif

#if defined(INCLUDE_KAFKA) || defined(LIVE_MODE)
  if (not device.empty() and live_mode) {
    hd_info(CYAN("采集网卡: "), device);
  }
  if (duration > 0) {
    hd_line(CYAN(", 持续 "), duration, " 秒");
  }
#endif

#if defined(LIVE_MODE) || defined(DEAD_MODE)
  if (this->write_file and not output_file.empty()) {
    hd_line(CYAN("输出文件:  "), output_file);
  }
#endif

#if defined(DEAD_MODE)
  if (this->offline_mode and not pcap_file.empty()) {
    hd_line(CYAN("输入文件:  "), pcap_file);
  }
#endif
}

hd::type::capture_option::~capture_option() {
  if (verbose) {
    print();
  }
}

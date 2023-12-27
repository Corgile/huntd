//
// Created by brian on 11/22/23.
//

#include <hound/common/macro.hpp>
#include <hound/type/capture_option.hpp>

void hd::type::capture_option::set_ipv4() {
  this->include_ip4 = true;
  auto const pos = this->filter.find_last_of(")") + 1;
  this->filter.insert(pos, "or ip");
}

void hd::type::capture_option::set_tcp() {
  this->include_tcp = true;
  // auto const pos = this->filter.find_last_of(")") + 1;
  // this->filter.insert(pos, "or ip");
}

void hd::type::capture_option::set_udp() {
  this->include_udp = true;
  auto const pos = this->filter.find_last_of(")") + 1;
  this->filter.insert(pos, "or udp");
}

// @formatter:off
void hd::type::capture_option::print() const {
  hd_info(CYAN("\n包含流量包的: "));
  if (include_eth)      hd_info("eth ");
  if (include_ip4)      hd_info("ipv4 ");
  if (include_ipv6)     hd_info("ipv6 ");
  if (include_tcp)      hd_info("tcp ");
  if (include_udp)      hd_info("udp ");
  if (include_icmp)     hd_info("icmp ");
  if (include_vlan)     hd_info("vlan ");
  if (payload > 0)      hd_info("头部, ", payload, " 字节payload\n");
  if (caplen)           hd_info(CYAN("报文长度, "));
  if (timestamp)        hd_info(CYAN("时间戳"));

  if (num_packets > 0)  hd_line(CYAN("抓包个数: "), num_packets);
  hd_line(CYAN("\n填充值: "), fill_bit);
  hd_info(CYAN("将每 "), stride, CYAN(" 位一组按"));
  if (unsign)           hd_line(YELLOW("无符号") CYAN("类型转换为10进制"));
  else                  hd_line(YELLOW("有符号") CYAN("类型转换为10进制"));
  hd_line(CYAN("包处理线程: "), workers);
  hd_line(CYAN("filter: "), filter);
  //@formatter:on

  if (this->write_file and not output_file.empty()) {
    hd_line(CYAN("输出文件:  "), output_file);
  }

  if (not pcap_file.empty()) {
    hd_line(CYAN("输入文件:  "), pcap_file);
  }
  hd_line("");
}

hd::type::capture_option::~capture_option() {
  if (verbose) {
    print();
  }
}

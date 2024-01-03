//
// Created by brian on 11/22/23.
//

#include <hound/common/macro.hpp>
#include <hound/type/capture_option.hpp>

// @formatter:off
void hd::type::capture_option::print() const {
  hd_info(CYAN("\n包含流量包的: "));
  if (payload > 0)      hd_info(payload, " 字节payload\n");
  if (caplen)           hd_info(CYAN("报文长度, "));
  if (timestamp)        hd_info(CYAN("时间戳"));

  if (num_packets > 0)  hd_line(CYAN("抓包个数: "), num_packets);
  hd_line(CYAN("\n填充值: "), fill_bit);
  hd_line(CYAN("流超时时间: "), flowTimeout, "秒");
  hd_info(CYAN("将每 "), stride, CYAN(" 位一组按"));
  hd_line(YELLOW("无符号") CYAN("类型转换为10进制"));
  hd_line(CYAN("包处理线程: "), workers);
  hd_line(CYAN("filter: "), filter);
  //@formatter:on

  hd_line(CYAN("kafka设置:  "), kafka_config);
  hd_line(CYAN("流最小包数: "), min_packets);
  hd_line(CYAN("流最大包数: "), max_packets);
  hd_info(CYAN("采集网卡: "), device);
  if (duration > 0) {
    hd_line(CYAN(", 持续 "), duration, " 秒");
  }
  hd_line("");
}

hd::type::capture_option::~capture_option() {
  if (verbose) {
    print();
  }
}

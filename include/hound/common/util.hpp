//
// Created by brian on 11/22/23.
//

#ifndef HOUND_UTILS_HPP
#define HOUND_UTILS_HPP

#include <filesystem>
#include <getopt.h>
#include <memory>
#include <string>
#include <hound/common/macro.hpp>
#include <hound/common/global.hpp>
#include <hound/type/capture_option.hpp>
#include <hound/type/parsed_data.hpp>

namespace hd::util {
namespace fs = std::filesystem;
using namespace hd::global;
using namespace hd::type;
inline char ByteBuffer[PCAP_ERRBUF_SIZE];

#pragma region ShortAndLongOptions
static option longopts[] = {
/// specify which network interface to capture
{"device", required_argument, nullptr, 'd'},
{"workers", required_argument, nullptr, 'J'},
{"duration", required_argument, nullptr, 'D'},
/// custom filter for libpcap
{"filter", required_argument, nullptr, 'F'},
{"fill", required_argument, nullptr, 'f'},
{"num-packets", required_argument, nullptr, 'N'},
/// min packets
{"min", required_argument, nullptr, 'L'},
/// max packets
{"max", required_argument, nullptr, 'R'},
/// threshold seconds(to determine whether to send)
{"interval", required_argument, nullptr, 'E'},
{"kafka", required_argument, nullptr, 'K'},
/// pcap file path, required when processing a pcapng file.
{"pcap-file", required_argument, nullptr, 'P'},
/// num of bits to convert as an integer
{"stride", required_argument, nullptr, 'S'},
/// dump output into a csv_path file
{"write", required_argument, nullptr, 'W'},
{"payload", required_argument, nullptr, 'p'},
/// no argument
#if defined(HD_FUTURE_SUPPORT)
    {"radiotap",    no_argument,       nullptr, 'r'},
    {"wlan",        no_argument,       nullptr, 'w'},
    {"eth",         no_argument,       nullptr, 'e'},
    {"ipv6",        no_argument,       nullptr, '6'},
    {"icmp",        no_argument,       nullptr, 'i'},
#endif
{"ipv4", no_argument, nullptr, '4'},
{"tcp", no_argument, nullptr, 't'},
{"udp", no_argument, nullptr, 'u'},
{"help", no_argument, nullptr, 'h'},
{"timestamp", no_argument, nullptr, 'T'},
{"caplen", no_argument, nullptr, 'C'},
{"verbose", no_argument, nullptr, 'V'},
{"unsigned", no_argument, nullptr, 'U'},
{nullptr, 0, nullptr, 0}
};
static char const* shortopts = "J:"
#if defined(LIVE_MODE)
  "d:D:"
#if defined(SEND_KAFKA)
  "K:E:"
#endif
#endif
#if defined(DEAD_MODE)
  "P:W:"
#endif
#if defined(HD_FUTURE_SUPPORT)
  "rwe6i"
#endif
  "F:f:N:S:L:R:p:C4uhTtVU";
#pragma endregion ShortAndLongOptions //@formatter:on

static void BuildFilter(capture_option& opt) {
  bool config_filter_set{false};
  if (not opt.filter.empty()) {
    opt.filter.append(" and(");
  } else { opt.include_ip4 = true; }

  if (opt.include_tcp or opt.include_udp or opt.include_icmp or opt.include_vlan) {
    opt.filter.append("(");
    if (opt.include_tcp) {
      opt.filter.append("tcp");
      config_filter_set = true;
    }
    if (opt.include_udp) {
      if (config_filter_set) opt.filter.append(" or ");
      opt.filter.append("udp");
      config_filter_set = true;
    }
    if (opt.include_icmp) {
      if (config_filter_set) opt.filter.append(" or ");
      opt.filter.append("icmp");
      config_filter_set = true;
    }
    opt.filter.append(")");
  }
  if (config_filter_set) {
    opt.filter.append("or");
  } else {
    opt.filter.append(")");
  }
  opt.filter.append("(vlan and(tcp or udp)))");

  if (not opt.include_ipv6) {
    /// not ipv6 and not icmp and not icmpv6
    //opt.filter.append("and not(ip6 or icmp6)");
  }
}

static void SetFilter(pcap_t* handle, std::string& device) {
  if (opt.filter.empty() or handle == nullptr) { return; }
  bpf_u_int32 net{0}, mask{0};
  bpf_program fp{};
#if defined(LIVE_MODE)
  if (opt.live_mode and not device.empty()) {
    if (pcap_lookupnet(device.c_str(), &net, &mask, ByteBuffer) == -1) {
      hd_line("获取设备掩码失败: ", device, ByteBuffer);
      exit(EXIT_FAILURE);
    }
  }
#endif
  hd_debug(opt.filter);
  if (pcap_compile(handle, &fp, opt.filter.c_str(), 0, net) == -1) {
    hd_line("解析 Filter 失败: ", opt.filter, "\n", pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }

  if (pcap_setfilter(handle, &fp) == -1) {
    hd_line("设置 Filter 失败: ", pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }
}

static pcap_t* OpenLiveHandle(capture_option& option) {
  /* getFlowId device */
  if (option.device.empty()) {
    pcap_if_t* l;
    int32_t const rv{pcap_findalldevs(&l, ByteBuffer)};
    if (rv == -1) {
      hd_line("找不到默认网卡设备", ByteBuffer);
      exit(EXIT_FAILURE);
    }
    option.device = l->name;
    pcap_freealldevs(l);
  }
  hd_debug(option.device);
  /* open device */
  auto const handle{pcap_open_live(option.device.c_str(), BUFSIZ, 1, 1000, ByteBuffer)};
  if (handle == nullptr) {
    hd_line("监听网卡设备失败: ", ByteBuffer);
    exit(EXIT_FAILURE);
  }
  /// apply filter
  BuildFilter(option);
  SetFilter(handle, option.device);
  pcap_set_promisc(handle, 1);
  pcap_set_buffer_size(handle, 25 << 22);
  // link_type = pcap_datalink(handle);
  // hd_debug(link_type);
  return handle;
}

#ifdef DEAD_MODE
static pcap_t* OpenDeadHandle(capture_option& option, uint32_t& link_type) {
  //using offline = pcap_t* (*)(const char*, u_int, char*);
  using offline = pcap_t* (*)(const char*, char*);
  //offline open_offline{pcap_open_offline_with_tstamp_precision};
  offline const open_offline{pcap_open_offline};
  if (not fs::exists(option.pcap_file)) {
    hd_line("无法打开文件 ", option.pcap_file);
    exit(EXIT_FAILURE);
  }
  //auto handle{open_offline(option.pcap_file.c_str(), PCAP_TSTAMP_PRECISION_NANO, hd::util::error_buffer)};
  auto const handle{open_offline(option.pcap_file.c_str(), ByteBuffer)};
  BuildFilter(option);
  SetFilter(handle, option.device);
  link_type = pcap_datalink(handle);
  return handle;
}
#endif // DEAD_MODE

static void Doc() {
  std::cout << "\t选项: " << shortopts << '\n';
  std::cout
#if defined(LIVE_MODE)
    << "\t-D, --duration       D秒后结束抓包\n"
    << "\t-d, --device         哪一个网卡\n"
    << "\t-N, --num-packets    指定抓包的数量\n"
#ifdef SEND_KAFKA
    << "\t-E, --interval       超时时间(新到达的packet距离上一个packet的时间)\n"
    << "\t-K, --kafka-config   kafka 配置文件路径\n"
#endif//SEND_KAFKA
#endif
    << "\t-J, --workers        处理流量包的线程数\n"
    << "\t-F, --filter         pcap filter (https://linux.die.net/man/7/pcap-filter)\n"
    << "\t-f, --fill           null字节填充值\n"
    << "\t-L, --min-packets    合并成流/json的时候，指定流的最 小 packet数量\n"
    << "\t-R, --max-packets    合并成流/json的时候，指定流的最 大 packet数量\n"
#if defined(DEAD_MODE)
    << "\t-P, --pcap-file      pcap文件路径, 处理离线 pcap,pcapng 文件\n"
    << "\t-W, --write          输出到文件, 需指定输出文件路径\n"
#endif//DEAD_MODE
    << "\t-S, --stride         将 S 位二进制串转换为 uint 数值(默认 8)\n"
    << "\t-4, --ipv4           包含 ipv4 报文\n"
    << "\t-t, --tcp            包含 tcp 报文\n"
    << "\t-T, --timestamp      包含时间戳(秒,毫秒)\n"
    << "\t-u, --udp            包含 udp 报文\n"
    << "\t-p, --payload        包含 n 字节的 payload\n"
#if defined(HD_FUTURE_SUPPORT)
    << "\t-r, --radiotap       包含 radiotap 报文(currently not supported)\n"
    << "\t-w, --wlan           包含 wlan 报文(currently not supported)\n"
    << "\t-e, --eth            包含 eth 报文\n"
    << "\t-6, --ipv6           包含 ipv6 报文(currently not supported)\n"
    << "\t-i, --icmp           包含 icmp 报文(currently not in plan)\n"
#endif//HD_FUTURE_SUPPORT
    << "\t-h, --help           用法帮助\n"
    << std::endl;
}

static void ParseOptions(capture_option& arguments, int argc, char* argv[]) {
  int longind = 0, option, j;
  opterr = 0;
  while ((option = getopt_long(argc, argv, shortopts, longopts, &longind)) not_eq -1) {
    switch (option) {
    case 'd': arguments.device = optarg;
      break;
    case 'C': arguments.caplen = true;
      break;
    case 'J': j = std::stoi(optarg);
      if (j < 1) {
        hd_line("worker 必须 >= 1");
        exit(EXIT_FAILURE);
      }
      arguments.workers = j;
      break;
#if defined(LIVE_MODE)
    case 'D': arguments.duration = std::stoi(optarg);
      break;
#endif
    case 'F': arguments.filter = optarg;
      break;
    case 'f':
      // try-except: fill = std::stoi(optarg);
      arguments.fill_bit = std::stoi(optarg);
      break;
    case 'N': arguments.num_packets = std::stoi(optarg);
      break;
#if defined(SEND_KAFKA) and defined(LIVE_MODE)
    case 'K': arguments.send_kafka = true;
      arguments.kafka_config = optarg;
#ifdef DEAD_MODE
      arguments.offline_mode = false;
#endif
      if (arguments.kafka_config.empty()) {
        hd_line("-k, --kafka-config 缺少值");
        exit(EXIT_FAILURE);
      }
      break;
#endif
    case 'p': arguments.payload = std::stoi(optarg);
      break;
    case 'S': arguments.stride = std::stoi(optarg);
      if (arguments.stride not_eq 1 and arguments.stride not_eq 8 and arguments.stride not_eq 16 and
        arguments.stride not_eq 32 and arguments.stride not_eq 64) {
        hd_line("-S,  --stride 只能是1, 8, 16, 32, 64, 现在是", arguments.stride);
        exit(EXIT_FAILURE);
      }
      break;
    // case 'I':
    //   arguments.output_index = std::stoi(optarg);
    //   if (arguments.output_index > 5 or arguments.output_index < 0) {
    //     hd_info("-I, --index 参数错误, 退出程序");
    //     exit(EXIT_FAILURE);
    //   }
    //   break;
    case 'L': arguments.min_packets = std::stoi(optarg);
      break;
    case 'R': arguments.max_packets = std::stoi(optarg);
      break;
    case 'E': arguments.packetTimeout = std::stoi(optarg);
      break;

#ifdef DEAD_MODE
    case 'W':
      arguments.write_file = true;
      arguments.output_file = optarg;
      if (optarg == nullptr or arguments.output_file.empty()) {
        hd_line("-W, --write 缺少值");
        exit(EXIT_FAILURE);
      }
      break;
    case 'P':
      arguments.offline_mode = true;
#if defined(LIVE_MODE)
      arguments.live_mode = false;
#endif
      arguments.pcap_file = optarg;
      if (arguments.pcap_file.empty()) {
        hd_line("-P, --pcap-file 缺少值");
        exit(EXIT_FAILURE);
      }
      break;
#endif

    case 'r': arguments.include_radiotap = true;
      break;
    case 'e': arguments.include_eth = true;
      break;
    case '4': arguments.include_ip4 = true;
      break;
    case 'u': arguments.include_udp = true;
      break;
    case 'i': arguments.include_icmp = true;
      break;
    case '6': arguments.include_ipv6 = true;
      break;
    case 'w': arguments.include_wlan = true;
      break;
    case 'h': hd::util::Doc();
      exit(EXIT_SUCCESS);
    case 't': arguments.include_tcp = true;
      break;
    case 'T': arguments.timestamp = true;
      break;
    case 'V': arguments.verbose = true;
      break;
    case 'U': arguments.unsign = true;
      break;
    case '?':
      hd_line("选项 ", '-', char(optopt), (" 的参数是必需的"));
      exit(EXIT_FAILURE);
    default: break;
    }
  }
}

template <typename T>
static int inline min(T _a, T _b) {
  return _a < _b ? _a : _b;
}
} // namespace hd::util
#endif //HOUND_UTILS_HPP

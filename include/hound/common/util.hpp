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
{"num", required_argument, nullptr, 'N'},
/// min packets
{"min", required_argument, nullptr, 'L'},
/// max packets
{"max", required_argument, nullptr, 'R'},
/// threshold seconds(to determine whether to send)
{"timeout", required_argument, nullptr, 'E'},
{"kafka-conf", required_argument, nullptr, 'K'},
/// num of bits to convert as an integer
{"stride", required_argument, nullptr, 'S'},
{"payload", required_argument, nullptr, 'p'},
{"help", no_argument, nullptr, 'h'},
{"timestamp", no_argument, nullptr, 'T'},
{"caplen", no_argument, nullptr, 'C'},
{"verbose", no_argument, nullptr, 'V'},
{nullptr, 0, nullptr, 0}
};
static char const *shortopts = "J:K:E:F:f:N:S:L:R:p:CTVh";
#pragma endregion ShortAndLongOptions //@formatter:on


static void SetFilter(pcap_t *handle, std::string &device) {
  if (opt.filter.empty() or handle == nullptr) { return; }
  bpf_u_int32 net{0}, mask{0};
  bpf_program fp{};
  if (not device.empty()) {
    if (pcap_lookupnet(device.c_str(), &net, &mask, ByteBuffer) == -1) {
      hd_line("获取设备掩码失败: ", device, ByteBuffer);
      exit(EXIT_FAILURE);
    }
  }
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

static pcap_t *OpenLiveHandle(capture_option &option) {
  /* getFlowId device */
  if (option.device.empty()) {
    pcap_if_t *l;
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
  SetFilter(handle, option.device);
  pcap_set_promisc(handle, 1);
  pcap_set_buffer_size(handle, 25 << 22);
  return handle;
}

static void Doc() {
  std::cout << "\t选项: " << shortopts << '\n';
  std::cout
    << "\t-D, --duration       D秒后结束抓包\n"
    << "\t-N, --num            指定抓包的数量\n"
    << "\t-E, --timeout        flow超时时间(新到达的packet距离上一个packet的时间)\n"
    << "\t-K, --kafka-conf     kafka 配置文件路径\n"
    << "\t-J, --workers        处理流量包的线程数\n"
    << "\t-F, --filter         pcap filter (https://linux.die.net/man/7/pcap-filter)\n"
    << "\t-f, --fill           null字节填充值\n"
    << "\t-L, --min            合并成流/json的时候，指定流的最 小 packet数量\n"
    << "\t-R, --max            合并成流/json的时候，指定流的最 大 packet数量\n"
    << "\t-S, --stride         将 S 位二进制串转换为 uint 数值(默认 8)\n"
    << "\t-T, --timestamp      包含时间戳(秒,毫秒)\n"
    << "\t-p, --payload        包含 n 字节的 payload\n"
    << "\t-h, --help           用法帮助\n"
    << std::endl;
}

static void ParseOptions(capture_option &arguments, int argc, char *argv[]) {
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
    case 'D': arguments.duration = std::stoi(optarg);
      break;
    case 'F': arguments.filter = optarg;
      break;
    case 'f':
      // try-except: fill = std::stoi(optarg);
      arguments.fill_bit = std::stoi(optarg);
      break;
    case 'N': arguments.num_packets = std::stoi(optarg);
      break;
    case 'K': arguments.kafka_config = optarg;
      if (arguments.kafka_config.empty()) {
        hd_line("-k, --kafka-config 缺少值");
        exit(EXIT_FAILURE);
      }
      break;
    case 'p': arguments.payload = std::stoi(optarg);
      break;
    case 'S': arguments.stride = std::stoi(optarg);
      if (arguments.stride not_eq 1 and arguments.stride not_eq 8 and arguments.stride not_eq 16 and
        arguments.stride not_eq 32 and arguments.stride not_eq 64) {
        hd_line("-S,  --stride 只能是1, 8, 16, 32, 64, 现在是", arguments.stride);
        exit(EXIT_FAILURE);
      }
      break;
    //   break;
    case 'L': arguments.min_packets = std::stoi(optarg);
      break;
    case 'R': arguments.max_packets = std::stoi(optarg);
      break;
    case 'E': arguments.flowTimeout = std::stoi(optarg);
      break;
    case 'h': Doc();
      exit(EXIT_SUCCESS);
    case 'T': arguments.timestamp = true;
      break;
    case 'V': arguments.verbose = true;
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

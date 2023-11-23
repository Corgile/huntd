//
// Created by brian on 11/22/23.
//

#ifndef HOUND_UTILS_HPP
#define HOUND_UTILS_HPP

#include <filesystem>
#include <getopt.h>
#include <memory>
#include <mutex>
#include <pcap/pcap.h>
#include <queue>
#include <string>
#include <vector>
#include <hound/common/hd_global.hpp>
#include <hound/common/hd_type.hpp>
#include <hound/entity/capture_option.hpp>
#include <dbg.h>

namespace hd::util {
	namespace fs = std::filesystem;
	using namespace hd::global;
	using namespace hd::entity;
	using namespace hd::type;
	static char error_buffer[PCAP_ERRBUF_SIZE];

#pragma region ShortAndLongOptions
	static char const* shortopts = "J:d:D:F:N:K:P:S:I:L:R:E:W:I:p:Cre4ui6whTtV";
	static option longopts[] = {
			/// specify which network interface to capture
			{"device",      required_argument, nullptr, 'd'},
			{"workers",     required_argument, nullptr, 'J'},
			{"duration",    required_argument, nullptr, 'D'},
			/// custom filter for libpcap
			{"filter",      required_argument, nullptr, 'F'},
			{"num-packets", required_argument, nullptr, 'N'},
			/// min packets
			{"min",         required_argument, nullptr, 'L'},
			/// max packets
			{"max",         required_argument, nullptr, 'R'},
			/// threshold seconds(to determine whether to send)
			{"interval",    required_argument, nullptr, 'E'},
			{"kafka-conf",  required_argument, nullptr, 'K'},
			/// pcap file path, required when processing a pcapng file.
			{"pcap-file",   required_argument, nullptr, 'P'},
			/// num of bits to convert as an integer
			{"stride",      required_argument, nullptr, 'S'},
			/// specify one of 0~5 integer as your flowId
			{"index",       required_argument, nullptr, 'I'},
			/// dump output into a csv_path file
			{"write",       required_argument, nullptr, 'W'},
			{"payload",     required_argument, nullptr, 'p'},
			/// no argument
			{"radiotap",    no_argument,       nullptr, 'r'},
			{"wlan",        no_argument,       nullptr, 'w'},
			{"eth",         no_argument,       nullptr, 'e'},
			{"ipv4",        no_argument,       nullptr, '4'},
			{"ipv6",        no_argument,       nullptr, '6'},
			{"tcp",         no_argument,       nullptr, 't'},
			{"udp",         no_argument,       nullptr, 'u'},
			{"icmp",        no_argument,       nullptr, 'i'},
			{"help",        no_argument,       nullptr, 'h'},
			{"timestamp",   no_argument,       nullptr, 'T'},
			{"caplen",      no_argument,       nullptr, 'C'},
			{"verbose",     no_argument,       nullptr, 'V'},
			{nullptr, 0,                       nullptr, 0}};
#pragma endregion

	static void processPacket(const raw_packet_info& data) {
		auto [pkthdr, packet] = data;
		if (pkthdr == nullptr or packet == nullptr) return;
		constexpr int8_t bits = 8;
		using buf_t = stride_ut<bits>;
		auto buf_arr = reinterpret_cast<buf_t*>(packet);
		auto len = pkthdr->caplen / (bits / 8);
		for (int i = 0; i < len; ++i) {
			std::cout << buf_arr[i].buffer << ",";
		}
		num_processed_packet++;
		std::cout << "\n";
	}

	static void build_filter(capture_option& opt) {
		bool config_filter_set{false};
		if (not opt.filter.empty()) {
			opt.filter.append(" and");
		} else { opt.include_tcp = true; }

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
		if (config_filter_set) opt.filter.append("or");
		opt.filter.append("(vlan and(tcp or udp))");

		if (not opt.include_ipv6) {
			/// not ipv6 and not icmp and not icmpv6
			//opt.filter.append("and not(ip6 or icmp6)");
		}
	}

	static bool endsWith(const std::string& str, const std::string& suffix) {
		if (suffix.length() > str.length()) return false;
		return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
	}

	static void setFilter(pcap_t* handle, std::string& device) {
		if (opt.filter.empty() or handle == nullptr) { return; }
		bpf_u_int32 net{0}, mask{0};
		bpf_program fp{};

		if (opt.live_mode and not device.empty()) {
			if (pcap_lookupnet(device.c_str(), &net, &mask, error_buffer) == -1) {
				dbg("获取设备掩码失败: ", device, error_buffer);
				exit(EXIT_FAILURE);
			}
		}
		dbg(opt.filter);
		if (pcap_compile(handle, &fp, opt.filter.c_str(), 0, net) == -1) {
			dbg("解析 Filter 失败: ", pcap_geterr(handle));
			exit(EXIT_FAILURE);
		}

		if (pcap_setfilter(handle, &fp) == -1) {
			dbg("设置 Filter 失败: ", pcap_geterr(handle));
			exit(EXIT_FAILURE);
		}
	}

	static pcap_t*
	openLiveHandle(capture_option& option, uint32_t& link_type) {
		/* getFlowId device */
		if (option.device.empty()) {
			pcap_if_t* l;
			int32_t rv{pcap_findalldevs(&l, hd::util::error_buffer)};
			if (rv == -1) {
				dbg("找不到默认网卡设备", hd::util::error_buffer);
				exit(EXIT_FAILURE);
			}
			option.device = l->name;
			pcap_freealldevs(l);
		}
		dbg(option.device);
		/* open device */
		auto handle{pcap_open_live(option.device.c_str(), BUFSIZ, 1, 1000,
															 hd::util::error_buffer)};
		if (handle == nullptr) {
			dbg("监听网卡设备失败: ", hd::util::error_buffer);
			exit(EXIT_FAILURE);
		}
		/// apply filter
		hd::util::build_filter(option);
		hd::util::setFilter(handle, option.device);
		link_type = pcap_datalink(handle);
		dbg(link_type);
		return handle;
	}

	static pcap_t*
	openDeadHandle(capture_option& option, uint32_t& link_type) {
		//using offline = pcap_t* (*)(const char*, u_int, char*);
		using offline = pcap_t* (*)(const char*, char*);
		//offline open_offline{pcap_open_offline_with_tstamp_precision};
		offline open_offline{pcap_open_offline};
		if (not fs::exists(option.pcap_file)) {
			dbg("无法打开文件 ", option.pcap_file);
			exit(EXIT_FAILURE);
		}
		//auto handle{open_offline(option.pcap_file.c_str(), PCAP_TSTAMP_PRECISION_NANO, hd::util::error_buffer)};
		auto handle{open_offline(option.pcap_file.c_str(), hd::util::error_buffer)};
		hd::util::build_filter(option);
		hd::util::setFilter(handle, option.device);
		link_type = pcap_datalink(handle);
		return handle;
	}

	static void show_help() {
		std::cout << "\n\t选项: [d:D:F:N:L:R:E:K:P:S:I:W:p:re4ui6whTt]\n\n";
		std::cout
				<< "\t-d, --device         哪一个网卡\n"
				<< "\t-J, --workers        哪一个网卡\n"
				<< "\t-D, --duration       D秒后结束抓包\n"
				<< "\t-F, --filter         pcap filter (https://linux.die.net/man/7/pcap-filter)\n"
				<< "\t-N, --num-packets    指定抓包的数量\n"
				<< "\t-L, --min-packets    流的最小packet数量\n"
				<< "\t-R, --max-packets    流的最大packet数量\n"
				<< "\t-E, --interval       超时时间(新到达的packet距离上一个packet的时间)\n"
				<< "\t-K, --kafka-config   kafka 配置文件路径\n"
				<< "\t-P, --pcap-file      pcap文件路径, 处理离线 pcap,pcapng 文件\n"
				<< "\t-S, --stride         将 S 位二进制串转换为 uint 数值(default 8)\n"
				<< "\t-W, --write          输出到文件, 需指定输出文件路径\n"
				<< "\t-I, --index          指定要输出的列, 可选:\n"
				<< "                                    0: 源IP \n"
				<< "                                    1: 目的IP\n"
				<< "                                    2: 源端口\n"
				<< "                                    3: 目的端口\n"
				<< "                                    4: 五元组(default)\n"
				<< "\t-r, --radiotap       包含 radiotap 报文(currently not supported)\n"
				<< "\t-w, --wlan           包含 wlan 报文(currently not supported)\n"
				<< "\t-e, --eth            包含 eth 报文\n"
				<< "\t-4, --ipv4           包含 ipv4 报文\n"
				<< "\t-6, --ipv6           包含 ipv6 报文(currently not supported)\n"
				<< "\t-t, --tcp            包含 tcp 报文\n"
				<< "\t-T, --timestamp      包含时间戳(秒,毫秒)\n"
				<< "\t-u, --udp            包含 udp 报文\n"
				<< "\t-i, --icmp           包含 icmp 报文(currently not in plan)\n"
				<< "\t-p, --payload-payload_len    包含 n 字节的 payload\n"
				<< "\t-h, --help           用法帮助\n"
				<< std::endl;
	}

	static void parse_options(capture_option& arguments, int argc, char* argv[]) {
		int longind = 0, option, j;
		opterr = 0;
		while ((option = getopt_long(argc, argv, shortopts, longopts, &longind)) not_eq -1) {
			switch (option) {
				case 'd':
					arguments.device = optarg;
					break;
				case 'C':
					arguments.caplen = true;
					break;
				case 'J':
					j = std::stoi(optarg);
					if (j < 1) {
						dbg("worker 必须 >= 1");
						exit(EXIT_FAILURE);
					}
					arguments.workers = j;
					break;
				case 'D':
					arguments.duration = std::stoi(optarg);
					break;
				case 'F':
					arguments.filter = optarg;
					break;
				case 'N':
					arguments.num_packets = std::stoi(optarg);
					break;
				case 'K':
					arguments.send_kafka = true;
					arguments.kafka_config = optarg;
					arguments.offline_mode = false;
					if (arguments.kafka_config.empty()) {
						dbg("-k, --kafka-config 缺少值");
						exit(EXIT_FAILURE);
					}
					break;
				case 'p':
					arguments.payload_len = std::stoi(optarg);
					break;
				case 'S':
					arguments.stride = std::stoi(optarg);
					if (arguments.stride not_eq 1 and arguments.stride not_eq 8 and arguments.stride not_eq 16 and
							arguments.stride not_eq 32 and arguments.stride not_eq 64) {
						dbg("-S,  --stride 只能是1, 8, 16, 32, 64, 现在是", arguments.stride);
						exit(EXIT_FAILURE);
					}
					break;
				case 'I':
					arguments.output_index = std::stoi(optarg);
					if (arguments.output_index > 5 or arguments.output_index < 0) {
						dbg("-I, --index 参数错误, 退出程序");
						exit(EXIT_FAILURE);
					}
					break;
				case 'L':
					arguments.min_packets = std::stoi(optarg);
					break;
				case 'R':
					arguments.max_packets = std::stoi(optarg);
					break;
				case 'E':
					arguments.interval = std::stoi(optarg);
					break;
				case 'W':
					arguments.write_file = true;
					arguments.out_path = optarg;
					if (optarg == nullptr or arguments.out_path.empty()) {
						dbg("-W, --write 缺少值");
						exit(EXIT_FAILURE);
					}
					break;
				case 'P':
					arguments.offline_mode = true;
					arguments.live_mode = false;
					arguments.pcap_file = optarg;
					if (arguments.pcap_file.empty()) {
						dbg("-P, --pcap-file 缺少值");
						exit(EXIT_FAILURE);
					}
					break;
				case 'r':
					arguments.include_radiotap = true;
					break;
				case 'e':
					arguments.include_eth = true;
					break;
				case '4':
					arguments.include_ipv4 = true;
					break;
				case 'u':
					arguments.include_udp = true;
					break;
				case 'i':
					arguments.include_icmp = true;
					break;
				case '6':
					arguments.include_ipv6 = true;
					break;
				case 'w':
					arguments.include_wlan = true;
					break;
				case 'h':
					hd::util::show_help();
					exit(EXIT_SUCCESS);
				case 't':
					arguments.include_tcp = true;
					break;
				case 'T':
					arguments.timestamp = true;
					break;
				case 'V':
					arguments.verbose = true;
					break;
				case '?':
					dbg("选项 ", '-', char(optopt), (" 的参数是必需的"));
					exit(EXIT_FAILURE);
				default:
					break;
			}
		}
	}

	template<typename T>
	void append_raw_data(std::string& vec, void* rawData, int32_t num_fields) {
		T* typedData = reinterpret_cast<T*>(rawData);
		for (int i = 0; i < num_fields; ++i) { vec.append(",").append(std::to_string(typedData[i])); }
	}

}// namespace hd::util
#endif //HOUND_UTILS_HPP

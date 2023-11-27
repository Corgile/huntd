//
// Created by brian on 11/22/23.
//

#ifndef FC_REFACTORED_FC_TYPES_HPP
#define FC_REFACTORED_FC_TYPES_HPP

#include <pcap/pcap.h>
#pragma region 协议栈头文件
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#pragma endregion 协议栈头文件
#include <algorithm>
#include <cstdint>
#include <string>

namespace hd::type {
	using byte_t = uint8_t;
	using packet_raw_t = const void;

	using hdr_eth_t = const ether_header;
	using hdr_tcp_t = const tcphdr;
	using hdr_ip_t = const iphdr;
	using hdr_ip_p = const iphdr*;
	using hdr_ipv6_t = const ip6_hdr;
	using hdr_udp_t = const udphdr;

	using packet_handler = void (*)(byte_t*, const pcap_pkthdr*, const byte_t*);

	struct vlan_header {
		/**
		 * @verbatim
		 * 0                 1
		 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
		 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 * |Prio |C|         VLAN ID     |
		 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 * @endverbatim
		 */
		uint16_t vlan;
		/** Ethernet type for next layer */
		uint16_t etherType;
	} __attribute__((__packed__));

	template<int8_t _size>
	struct stride_t {
		int64_t buffer: _size;
	}__attribute__((__packed__));


	struct raw_packet_info {
		/// don't try to release those 2 pointers
		/// because pcap does this internally and automatically
		pcap_pkthdr* info_hdr;
		byte_t* byte_arr;

		raw_packet_info() = default;

		raw_packet_info(const pcap_pkthdr* pkthdr, const byte_t* packet) {
			this->info_hdr = const_cast<pcap_pkthdr*>(pkthdr);
			this->byte_arr = const_cast<byte_t*>(packet);
		}
	};

	struct ByteArray {
		byte_t* data;
		int32_t byteLen = 0;

		ByteArray() = default;

		/// list initializer
		ByteArray(byte_t* data, int32_t const byte_len)
			: data(data),
			  byteLen(byte_len) {
		}

		template<typename stride_t>
		std::string dump() {
			if (byteLen <= 0) return {};
			std::string ret;
			auto arr = reinterpret_cast<stride_t*>(data);
			for (int i = 0; i < byteLen - 1; ++i) {
				auto v = reinterpret_cast<decltype(stride_t::buffer)>(arr[i]);
				ret.append(std::to_string(v)).append(",");
			}
			auto v = reinterpret_cast<decltype(stride_t::buffer)>(arr[byteLen - 1]);
			ret.append(std::to_string(v));
			return ret;
		}
	};

	template<typename T>
	struct MyValuePair {
		T minVal, maxVal;

		MyValuePair() = default;

		MyValuePair(T& _v1, T& _v2)
			: minVal{std::min(_v1, _v2)},
			  maxVal{std::max(_v1, _v2)} {
			// std::tie(std::minmax({v1, v2}));
		}

		template<typename U, typename V>
		MyValuePair(const std::pair<U, V>& pair)
			: minVal{pair.first}, maxVal{pair.second} {
		}
	};

	enum sink {
		/// write to json file
		JSON_FILE = 0x1,
		/// write to csv/ascii text file
		TEXT_FILE = 0x2,
		/// send to some message queue (kafka)
		MSG_QUEUE = 0x4,
		/// print to console
		CONSOLE = 0x8,
		/// silent
		SILENT = -1,
	};
} //entity
#endif //FC_REFACTORED_FC_TYPES_HPP

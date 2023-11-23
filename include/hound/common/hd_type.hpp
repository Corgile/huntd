//
// Created by brian on 11/22/23.
//

#ifndef FC_REFACTORED_FC_TYPES_HPP
#define FC_REFACTORED_FC_TYPES_HPP

#include <cstdint>

namespace hd::type {
	using byte_t = unsigned char;
	using packet_raw_t = const void;

	using hdr_eth_t = const struct ether_header;
	using hdr_tcp_t = const struct tcphdr;
	using hdr_icmp_t = const struct icmphdr;
	using hdr_radiotap_t = const struct radiotap_header;
	using hdr_ip_t = const struct iphdr;
	using hdr_ip_p = const struct iphdr*;
	using hdr_ipv6_t = const struct ip6_hdr;
	using hdr_wlan_t = const struct wlan_header;
	using hdr_udp_t = const struct udphdr;

	using packet_ip = const struct ip;
	using packet_icmp = const struct icmp;

	using packet_handler = void (*)(byte_t*, const struct pcap_pkthdr*, const byte_t*);

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
	struct stride_st {
		int64_t buffer: _size;
	}__attribute__((__packed__));

	template<int8_t _size>
	struct stride_ut {
		uint64_t buffer: _size;
	}__attribute__((__packed__));

	struct SomeData {
	};

	struct raw_packet_info {
		/// don't try to release those 2 pointers
		/// because pcap does this internally and automatically
		pcap_pkthdr* pPcapHeader;
		byte_t* pRawPacket;

		raw_packet_info() = default;

		raw_packet_info(const pcap_pkthdr* pkthdr, const byte_t* packet) {
			this->pPcapHeader = const_cast<pcap_pkthdr*>(pkthdr);
			this->pRawPacket = const_cast<byte_t*>(packet);
		}
	};
}
#endif //FC_REFACTORED_FC_TYPES_HPP

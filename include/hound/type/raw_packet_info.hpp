//
// Created by brian on 11/28/23.
//

#ifndef HOUND_RAW_PACKET_INFO_HPP
#define HOUND_RAW_PACKET_INFO_HPP

#include <pcap/pcap.h>

namespace hd::type {

	using byte_t = uint8_t;

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

} // entity

#endif //HOUND_RAW_PACKET_INFO_HPP

//
// Created by brian on 11/22/23.
//

#include <thread>
#include <hound/parser/live_parser.hpp>
#include <hound/common/hd_global.hpp>
#include <hound/common/hd_util.hpp>

using namespace hd::global;

hd::entity::LiveParser::LiveParser() {
	this->mHandle = hd::util::openLiveHandle(opt, this->mLinkType);
}

void hd::entity::LiveParser::startCapture() {
	if (opt.duration > 0) {
		/// canceler thread
		std::thread([this]() {
			std::this_thread::sleep_for(
					std::chrono::seconds(opt.duration));
			pcap_breakloop(this->mHandle);
		}).detach();
	}
	for (int i = 0; i < opt.workers; ++i) {
		std::thread(&LiveParser::consumer_job, this).detach();
	}
	pcap_loop(mHandle, opt.num_packets, livePacketHandler, reinterpret_cast<u_char*>(this));
	pcap_close(mHandle);
}

hd::entity::LiveParser::~LiveParser() {
#if defined(BENCHMARK)
	dbg(num_captured_packet);
	dbg(num_missed_packet);
	dbg(num_consumed_packet);
	std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
}

void LiveParser::livePacketHandler(u_char* user_data, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
	auto _this{reinterpret_cast<LiveParser*>(user_data)};
	_this->rawPacketQueue.push({pkthdr, packet});
	std::cout << __PRETTY_FUNCTION__ << std::endl;
#if defined(BENCHMARK)
	num_captured_packet++;
#endif
}

void LiveParser::consumer_job() {
	while (true) {
		raw_packet_info packetInfo{nullptr, nullptr};
		this->rawPacketQueue.pop(packetInfo);
		this->processPacket(packetInfo);
	}
}

void LiveParser::processPacket(const raw_packet_info& data) {
	auto [pkthdr, packet] = data;

}

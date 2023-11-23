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
			this->stopCapture();
		}).detach();
	}
	for (int i = 0; i < opt.workers; ++i) {
		std::thread(&LiveParser::consumer_job, this).detach();
	}
	pcap_loop(mHandle, opt.num_packets, liveHandler, reinterpret_cast<u_char*>(this));
	pcap_close(mHandle);
}


void LiveParser::stopCapture() const {
	pcap_breakloop(this->mHandle);
}

void LiveParser::liveHandler(u_char* user_data, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
	auto _this{reinterpret_cast<LiveParser*>(user_data)};
	_this->rawPacketQueue.push({pkthdr, packet});
#if defined(BENCHMARK)
	num_captured_packet++;
#endif
}

void LiveParser::consumer_job() {
	/// 采用标志变量keepRunning来控制detach的线程
	while (keepRunning.load(std::memory_order_acquire)) {
		raw_packet_info packetInfo{nullptr, nullptr};
		if (this->rawPacketQueue.empty()) continue;
		this->rawPacketQueue.pop(packetInfo);
		hd::util::processPacket(packetInfo);
	}
}

hd::entity::LiveParser::~LiveParser() {
#if defined(BENCHMARK)
	dbg(num_captured_packet);
	dbg(num_missed_packet);
	dbg(num_consumed_packet);
	dbg(num_processed_packet);
#endif
	/// 先等待游离worker线程消费队列直至为空
	while (not this->rawPacketQueue.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(5));
	}
	/// 再控制游离线程停止访问主线程的资源
	keepRunning.store(false, std::memory_order_release);
//	exit(EXIT_SUCCESS);
}


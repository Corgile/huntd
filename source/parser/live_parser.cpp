//
// Created by brian on 11/22/23.
//

#include <thread>
#include <hound/parser/live_parser.hpp>
#include <hound/common/global.hpp>
#include <hound/common/util.hpp>

using namespace hd::global;

LiveParser::LiveParser() {
	this->mHandle = util::openLiveHandle(opt, this->mLinkType);
}

void LiveParser::startCapture() {
	if (opt.duration > 0) {
		/// canceler thread
		std::thread([this] {
			std::this_thread::sleep_for(
					std::chrono::seconds(opt.duration));
			this->stopCapture();
		}).detach();
	}
	for (int i = 0; i < opt.workers; ++i) {
		std::thread(&LiveParser::consumer_job, this).detach();
	}
	pcap_loop(mHandle, opt.num_packets, liveHandler, reinterpret_cast<byte_t*>(this));
	pcap_close(mHandle);
}


void LiveParser::stopCapture() const {
	pcap_breakloop(this->mHandle);
}

void LiveParser::liveHandler(byte_t* user_data, const pcap_pkthdr* pkthdr, const byte_t* packet) {
	auto const _this{reinterpret_cast<LiveParser*>(user_data)};
	std::ignore = _this->lockFreeQueue.push({pkthdr, packet});
#if defined(BENCHMARK)
	num_captured_packet++;
#endif //BENCHMARK
}

void LiveParser::consumer_job() {
	/// 采用标志变量keepRunning来控制detach的线程
	while (keepRunning.load(std::memory_order_acquire)) {
		raw_packet_info packetInfo{nullptr, nullptr};
		if (not this->lockFreeQueue.pop(packetInfo)) continue;
		util::processPacket(packetInfo, this->mLinkType, sink::CONSOLE);
	}
}

hd::entity::LiveParser::~LiveParser() {
#if defined(BENCHMARK)
	hd_info_one(num_captured_packet);
	hd_info_one(num_missed_packet);
	hd_info_one(num_consumed_packet);
	hd_info_one(num_processed_packet);
#endif
	/// 先等待游离worker线程消费队列直至为空
	while (not this->lockFreeQueue.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(5));
	}
	/// 再控制游离线程停止访问主线程的资源
	keepRunning.store(false, std::memory_order_release);
	hd_debug(this->lockFreeQueue.size());
}

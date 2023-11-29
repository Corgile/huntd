//
// Created by brian on 11/22/23.
//
#include <thread>

#include <hound/parser/live_parser.hpp>
#include <hound/common/global.hpp>
#include <hound/common/util.hpp>
#include <hound/common/macro.hpp>

#include <hound/sink/impl/text_file_sink.hpp>

hd::type::LiveParser::LiveParser() {
  this->mHandle = util::openLiveHandle(global::opt, this->mLinkType);
  mSink.reset(new TextFileSink(global::opt.out_path));
//  processor.reset(new BaseProcessor());
}

void hd::type::LiveParser::startCapture() {
  using namespace hd::global;
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

void hd::type::LiveParser::stopCapture() const {
  pcap_breakloop(this->mHandle);
}

void hd::type::LiveParser::liveHandler(byte_t* user_data, const pcap_pkthdr* pkthdr, const byte_t* packet) {
  auto const _this{reinterpret_cast<LiveParser*>(user_data)};
  std::ignore = _this->lockFreeQueue.push({pkthdr, packet});
#if defined(BENCHMARK)
  global::num_captured_packet++;
#endif //BENCHMARK
}

void hd::type::LiveParser::consumer_job() {
  /// 采用标志变量keepRunning来控制detach的线程
  while (keepRunning.load(std::memory_order_acquire)) {
    raw_packet_info packetInfo{nullptr, nullptr};
    if (not this->lockFreeQueue.pop(packetInfo)) continue;
    mSink->consumeData({packetInfo});
  }
}

hd::type::LiveParser::~LiveParser() {
#if defined(BENCHMARK)
  using namespace global;
  hd_info_one(num_captured_packet);
  hd_info_one(num_missed_packet);
  hd_info_one(num_consumed_packet);
  hd_info_one(num_processed_packet);
#endif//- #if defined(BENCHMARK)
  /// 先等待游离worker线程消费队列直至为空
  while (not this->lockFreeQueue.empty()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
  /// 再控制游离线程停止访问主线程的资源
  keepRunning.store(false, std::memory_order_release);
  hd_debug(this->lockFreeQueue.size());
}

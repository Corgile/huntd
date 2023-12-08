//
// Created by brian on 11/22/23.
//
#if defined(DEAD_MODE)

#include <hound/common/util.hpp>
#include <hound/common/macro.hpp>
#include <hound/parser/dead_parser.hpp>
#include <hound/sink/impl/json_file_sink.hpp>
#include <hound/sink/impl/text_file_sink.hpp>

#if defined(BENCHMARK)

#include <hound/type/timer.hpp>

#endif

hd::type::DeadParser::DeadParser() {
  this->mHandle = util::OpenDeadHandle(global::opt, this->mLinkType);
  if (global::opt.output_file.empty()) {
    mSink.reset(new BaseSink(global::opt.output_file));
    return;
  }
  if (global::opt.output_file.ends_with(".json")) {
    mSink.reset(new JsonFileSink(global::opt.output_file));
  }
  else {
    mSink.reset(new TextFileSink(global::opt.output_file));
  }
}

void hd::type::DeadParser::processFile() {
#if defined(BENCHMARK)
  Timer timer(_timeConsumption_ms);
#endif
  using namespace hd::global;
  for (int i = 0; i < opt.workers; ++i) {
    std::thread(&DeadParser::consumer_job, this).detach();
  }
  pcap_loop(mHandle, opt.num_packets, deadHandler, reinterpret_cast<byte_t*>(this));
}

void hd::type::DeadParser::deadHandler(byte_t* user_data, const pcap_pkthdr* pkthdr, const byte_t* packet) {
  auto const _this{reinterpret_cast<DeadParser*>(user_data)};
  std::unique_lock _accessToQueue(_this->mQueueLock);
  _this->mPacketQueue.push({
  pkthdr,
  packet,
  util::min<int>(global::opt.payload + 128, static_cast<int>(pkthdr->caplen))
  });
  _accessToQueue.unlock();
  _this->cv_consumer.notify_all();
#if defined(BENCHMARK)
  ++global::num_captured_packet;
#endif //BENCHMARK
}

void hd::type::DeadParser::consumer_job() {
  /// 采用标志变量keepRunning来控制detach的线程
  while (keepRunning) {
    std::unique_lock lock(this->mQueueLock);
    this->cv_consumer.wait(lock, [this] {
      return not this->mPacketQueue.empty() or not keepRunning;
    });
    if (not keepRunning) break;
    if (this->mPacketQueue.empty()) continue;
    raw_packet_info packetInfo{this->mPacketQueue.front()};
    this->mPacketQueue.pop();
    lock.unlock();
    cv_producer.notify_one();
    mSink->consumeData({packetInfo});
#if defined(BENCHMARK)
    ++global::num_consumed_packet;
#endif
  }
}

hd::type::DeadParser::~DeadParser() {
  /// 先等待游离worker线程消费队列直至为空
  while (not this->mPacketQueue.empty()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  /// 再控制游离线程停止访问主线程的资源
  keepRunning.store(false);
#if defined(BENCHMARK)
  using namespace global;
  hd_line(CYAN("num_captured_packet = "), num_captured_packet.load());
  hd_line(CYAN("num_dropped_packets = "), num_dropped_packets.load());
  hd_line(CYAN("num_consumed_packet = "), num_consumed_packet.load());
  hd_line(CYAN("num_written_csv = "), num_written_csv.load());
  hd_line(CYAN("_timeConsumption_ms = "), _timeConsumption_ms);
#endif//- #if defined(BENCHMARK)
  hd_debug(this->mPacketQueue.size());
  ///- 最好不要强制exit(0), 因为还有worker在死等。
  // exit(EXIT_SUCCESS);
  cv_consumer.notify_all();
}

#endif

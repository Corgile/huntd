//
// Created by brian on 11/22/23.
//

#ifndef FC_REFACTORED_LIVE_PARSER_HPP
#define FC_REFACTORED_LIVE_PARSER_HPP

#include <pcap/pcap.h>
#include <atomic>
// #include <hound/type/lock_free_queue.hpp>
#include <hound/type/lock_free_queue_v.hpp>
#include <hound/type/raw_packet_info.hpp>
#include <hound/sink/base_sink.hpp>
// #include <hound/type/concurrent_queue.hpp>

namespace hd::type {

class LiveParser {
public:
  explicit LiveParser();

  void startCapture();

  void stopCapture() const;

  ~LiveParser();

private:

  static void liveHandler(u_char*, const pcap_pkthdr*, const u_char*);

  void consumer_job();

private:


private:
  pcap_t* mHandle{nullptr};
  uint32_t mLinkType{};
  entity::LockFreeQueue<raw_packet_info, 8192> lockFreeQueue;
  // moodycamel::ConcurrentQueue<raw_packet_info> lockFreeQueue;
  std::atomic<bool> keepRunning{true};
  std::shared_ptr<BaseSink> mSink;


};

} // entity

#endif //FC_REFACTORED_LIVE_PARSER_HPP

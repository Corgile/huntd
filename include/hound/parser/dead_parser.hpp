//
// Created by brian on 11/22/23.
//

#ifndef FC_REFACTORED_DEAD_PARSER_HPP
#define FC_REFACTORED_DEAD_PARSER_HPP

#include <condition_variable>
#include <hound/sink/impl/text_file_sink.hpp>
#include <hound/sink/impl/json_file_sink.hpp>

namespace hd::type {
class DeadParser {

public:
  DeadParser();

  void processFile();

  virtual ~DeadParser();

private:
  static void deadHandler(u_char*, const pcap_pkthdr*, const u_char*);

  void consumer_job();

private:
  pcap_t* mHandle{nullptr};
  uint32_t mLinkType{};
  std::queue<raw_packet_info> mPacketQueue;
  std::atomic<bool> keepRunning{true};
  std::shared_ptr<BaseSink> mSink;
  std::condition_variable cv_producer;      // 生产者条件变量
  std::condition_variable cv_consumer;      // 消费者条件变量
  mutable std::mutex mQueueLock;
  double _timeConsumption_ms = 0.;
};

}


#endif //FC_REFACTORED_DEAD_PARSER_HPP

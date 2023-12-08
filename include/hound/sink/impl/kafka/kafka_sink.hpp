//
// Created by brian on 11/22/23.
//

#ifndef HOUND_KAFKA_HPP
#define HOUND_KAFKA_HPP

#include <hound/sink/impl/kafka/kafka_config.hpp>
#include <hound/sink/impl/kafka/connection_pool.hpp>
#include <hound/sink/impl/kafka/kafka_connection.hpp>

#include <hound/sink/impl/flow_check.hpp>

namespace hd::type {

using namespace hd::entity;
using namespace hd::global;

using PacketList = std::vector<hd_packet>;

class KafkaSink final : public BaseSink {
public:
  explicit KafkaSink(std::string const& fileName) {
    kafka_config kafkaConfig;
    flow::LoadKafkaConfig(kafkaConfig, fileName);
    this->mConnectionPool.reset(connection_pool::create(kafkaConfig));
    std::thread(&KafkaSink::sendingJob, this).detach();
  }

  ~KafkaSink() override {
    mIsRunning = false;
    cvMsgSender.notify_all();
    hd_debug(__PRETTY_FUNCTION__);
    hd_debug(this->mFlowTable.size());
  }

  void consumeData(ParsedData const& data) override {
    if (not data.HasContent) return;
    hd_packet packet(data.mPcapHead);
    this->fillRawBitVec(data, packet.bitvec);
    std::scoped_lock mapLock(mtxAccessToMap);
    PacketList packetList{mFlowTable[data.mFlowKey]};
    if (flow::IsFlowReady(packetList, packet)) {
      std::scoped_lock queueLock(mtxAccessToQueue);
      mSendQueue.emplace(data.mFlowKey, std::move(mFlowTable.at(data.mFlowKey)));
      cvMsgSender.notify_all();
    }
    mFlowTable.at(data.mFlowKey).emplace_back(std::move(packet));
  }

private:

  void sendingJob() {
    while (mIsRunning) {
      std::unique_lock lock(mtxAccessToQueue);
      cvMsgSender.wait(lock, [&]() {
        return not this->mSendQueue.empty() or not mIsRunning;
      });
      if (not mIsRunning) break;
      auto front{this->mSendQueue.front()};
      this->mSendQueue.pop();
      lock.unlock();
      this->send(front);
    }
    hd_debug(YELLOW("void sendingJob() 结束"));
  }

  void send(const hd_flow& flow) const {
    if (flow.count < opt.min_packets) return;
    std::string payload;
    struct_json::to_json(flow, payload);
    auto const connection{mConnectionPool->get_connection()};
    connection->pushMessage(payload, flow.flowId);
  }

  void sendTheRest() {
    if (mFlowTable.empty()) return;
    for (auto& [k, list]: mFlowTable) {
      this->send({k, list});
    }
    mFlowTable.clear();
  }

private:
  std::mutex mtxAccessToMap;
  std::map<std::string, PacketList> mFlowTable;

  std::mutex mtxAccessToQueue;
  std::queue<hd_flow> mSendQueue;
  std::condition_variable cvMsgSender;

  std::unique_ptr<connection_pool> mConnectionPool;
  std::atomic_bool mIsRunning{true};
};
} // entity

#endif //HOUND_KAFKA_HPP

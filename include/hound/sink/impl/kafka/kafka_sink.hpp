//
// Created by brian on 11/22/23.
//

#ifndef HOUND_KAFKA_HPP
#define HOUND_KAFKA_HPP

#include <fstream>
#include <hound/sink/impl/kafka/kafka_config.hpp>
#include <hound/sink/impl/kafka/connection_pool.hpp>
#include <hound/sink/impl/kafka/kafka_connection.hpp>

namespace hd::type {
using PacketList = std::vector<entity::hd_packet>;

class KafkaSink : public BaseSink {
public:
  KafkaSink(std::string const& fileName) {
    std::ifstream config_file(fileName);
    if (not config_file.is_open()) {
      hd_info(RED("无法打开配置文件: "), fileName);
      exit(EXIT_FAILURE);
    }
    std::string line;
    while (getline(config_file, line)) {
      if (line.empty() or line[0] == '#') {
        continue;
      }
      size_t pos{line.find('=')};
      if (pos == std::string::npos) {
        hd_info(RED("配置项格式错误: "), line);
        exit(EXIT_FAILURE);
      }
      auto key{line.substr(0, pos)};
      auto value{line.substr(pos + 1)};
      if (value.empty()) continue;
      this->config[key] = value;
      hd_info(BLUE("加载配置: "), key, "=", value);
    }
    hd::entity::kafka_config kafkaConfig = {
        .conn = {
            .servers = config.find(hd::keys::KAFKA_BROKERS)->second,
            .topics = config[hd::keys::KAFKA_TOPICS],
            .partition = 0,
            .max_idle = 200,
            .timeout_sec = 1200,
        },
        .pool = {
            .init_size = 10,
            .max_size = 100,
        }
    };
    this->mConnectionPool = hd::entity::connection_pool::create(kafkaConfig);
  }

  ~KafkaSink() override {
    delete mConnectionPool;
  }

  /// 写入发送到kafka
  void consumeData(ParsedData const& data) override {
    if (not data.HasContent) return;
    using namespace hd::entity;
    using namespace hd::global;
    std::string buffer;
    this->fillRawBitVec(data, buffer);
    hd_packet packet{
        .ts_sec=data.mPcapHead.ts_sec,
        .ts_usec=data.mPcapHead.ts_usec,
        .packet_len=data.mPcapHead.caplen,
        .bitvec=std::move(buffer)
    };
    {
      std::lock_guard<std::mutex> lock(mAccessToFlowTable);
      auto _packetList = this->mFlowTable[data.mFlowKey];
      if (_packetList.empty()) goto merge_into_existing_flow;
      if (okToRemove(_packetList, data)) {
        if (_packetList.size() >= opt.min_packets) {
          this->send({
                         data.mFlowKey,
                         (int) _packetList.size(),
                         std::move(_packetList)
                     }
          );
        }
        mFlowTable.erase(data.mFlowKey);
      }
merge_into_existing_flow:
      this->mFlowTable[data.mFlowKey].emplace_back(packet);
    }
  }

  void send(const entity::hd_flow& flow) {
    std::string payload;
    struct_json::to_json(flow, payload);
    this->mConnectionPool->get_connection()->pushMessage(payload, flow.flowId);
  }

private:
  bool inline okToRemove(PacketList const& list, ParsedData const& data) {
    using namespace global;
    return data.mPcapHead.ts_sec - list.back().ts_sec >= opt.interval
           or list.size() >= opt.max_packets;
  }

private:
  std::map<std::string, PacketList> mFlowTable;
  std::mutex mAccessToFlowTable;
  std::unordered_map<std::string, std::string> config;
  hd::entity::connection_pool* mConnectionPool;
};

} // entity

#endif //HOUND_KAFKA_HPP

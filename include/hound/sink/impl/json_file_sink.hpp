//
// Created by brian on 11/22/23.
//

#ifndef HOUND_JSON_FILE_SINK_HPP
#define HOUND_JSON_FILE_SINK_HPP

#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include <struct_json/json_writer.h>
#include <hound/sink/base_sink.hpp>
#include <hound/type/hd_flow.hpp>
#include <hound/type/synced_stream.hpp>

namespace hd::type {
namespace fs = std::filesystem;

class JsonFileSink : public BaseSink {
  using PacketList = std::vector<entity::hd_packet>;
public:
  JsonFileSink(std::string const& fileName) :
      mOutFile(fileName, std::ios::out) {
    auto parent = absolute(fs::path(fileName)).parent_path();
    if (not fs::exists(parent)) {
      fs::create_directories(parent);
    }
    bool isGood = mOutFile.invoke([](std::fstream& stream) {
      return stream.good();
    });

    if (not isGood) {
      hd_info(RED("无法打开输出文件: "), fileName);
      exit(EXIT_FAILURE);
    }
    mOutFile << "[";
    mOutFile << std::flush;
  }

  /// 写入json文件
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
          hd_flow flow{data.mFlowKey, std::move(_packetList)};
          std::string _jsonStr;
          struct_json::to_json(flow, _jsonStr);
          this->append(_jsonStr);
        }
        mFlowTable.erase(data.mFlowKey);
      }
merge_into_existing_flow:
      this->mFlowTable[data.mFlowKey].emplace_back(packet);
    }
  }

  ~JsonFileSink() {
    std::streampos currentPosition = mOutFile.SyncInvoke(
        [](std::fstream& stream) {
          return stream.tellg();
        }
    );
    if (currentPosition > 0) {
      mOutFile.SyncInvoke(
          [&](std::fstream& stream) {
            stream.seekg(currentPosition.operator-(1));
          }
      );
      mOutFile << "]";
    }
  }

private:
  void append(std::string& content) {
    content.append(",");
    mOutFile << content;
  }

  bool inline okToRemove(PacketList const& list, ParsedData const& data) {
    return list.back().ts_sec - data.mPcapHead.ts_sec >= global::opt.interval or
           list.size() == global::opt.max_packets;
  }

private:
  SyncedStream<std::fstream> mOutFile;
  std::map<std::string, PacketList> mFlowTable;
  std::mutex mAccessToFlowTable;
};

} // entity

#endif //HOUND_JSON_FILE_SINK_HPP

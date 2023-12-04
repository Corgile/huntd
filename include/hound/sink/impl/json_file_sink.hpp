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
          this->append({
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

  ~JsonFileSink() {
    for (auto& [id, _list]: this->mFlowTable) {
      if (_list.size() >= global::opt.min_packets) {
        this->append({id, (int) _list.size(), std::move(_list)});
      }
      mFlowTable.erase(id);
    }
    std::streampos currentPosition = mOutFile.SyncInvoke(
        [](std::fstream& stream) { return stream.tellg(); }
    );
    if (currentPosition > 0) {
      mOutFile.SyncInvoke(
          [&](std::fstream& stream) {
            /// Subtract 2 here because there are ',' and '\\n' at the end.
            stream.seekg(currentPosition.operator-(2));
          }
      );
      mOutFile << "]";
    }
    hd_debug(mFlowTable.size());
  }

protected:
  void append(const entity::hd_flow& flow) {
    std::string content;
    struct_json::to_json(flow, content);
    content.append(",");
    mOutFile << content;
  }

private:
  bool inline okToRemove(PacketList const& list, ParsedData const& data) {
    using namespace global;
    return data.mPcapHead.ts_sec - list.back().ts_sec >= opt.interval
           or list.size() >= opt.max_packets;
  }

private:
  SyncedStream<std::fstream> mOutFile;
  std::map<std::string, PacketList> mFlowTable;
  std::mutex mAccessToFlowTable;
};

} // entity

#endif //HOUND_JSON_FILE_SINK_HPP

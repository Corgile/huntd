//
// Created by brian on 11/22/23.
//

#ifndef HOUND_JSON_FILE_SINK_HPP
#define HOUND_JSON_FILE_SINK_HPP

#include <mutex>
#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include <hound/type/hd_flow.hpp>
#include <hound/type/synced_stream.hpp>
#include <struct_json/json_writer.h>
#include <hound/sink/base_sink.hpp>
#include <hound/sink/impl/flow_check.hpp>

namespace hd::type {
namespace fs = std::filesystem;

class JsonFileSink : public BaseSink {
  using PacketList = std::vector<entity::hd_packet>;
public:
  JsonFileSink(std::string const& fileName) :
      mOutFile(fileName, std::ios::out) {
    auto parent{absolute(fs::path(fileName)).parent_path()};
    if (not fs::exists(parent)) {
      fs::create_directories(parent);
    }
    bool isGood{mOutFile.invoke([](std::fstream& stream) {
      return stream.good();
    })};

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
    hd::entity::hd_packet packet(data.mPcapHead);
    this->fillRawBitVec(data, packet.bitvec);
    std::scoped_lock mapLock(mAccessToFlowTable);
    PacketList packetList{mFlowTable[data.mFlowKey]};
    if (flow::IsFlowReady(packetList, packet)) {
      this->append(data.mFlowKey, std::move(mFlowTable.at(data.mFlowKey)));
    }
    mFlowTable.at(data.mFlowKey).emplace_back(std::move(packet));
  }

  ~JsonFileSink() {
    for (auto& [id, _list]: this->mFlowTable) {
      if (_list.size() >= global::opt.min_packets) {
        this->append(id, _list);
      }
    }
    mFlowTable.clear();
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

  template<typename ...Args>
  void append(Args&& ... args) {
    std::string content;
    hd::entity::hd_flow flow(std::forward<Args>(args)...);
    struct_json::to_json(flow, content);
    content.append(",");
    mOutFile << content;
  }

private:
  SyncedStream<std::fstream> mOutFile;
  std::map<std::string, PacketList> mFlowTable;
  std::mutex mAccessToFlowTable;
};

} // entity

#endif //HOUND_JSON_FILE_SINK_HPP

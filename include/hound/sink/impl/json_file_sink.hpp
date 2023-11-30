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
    mOutFile << "[" << std::flush;
  }

  /// 写入json文件
  void consumeData(ParsedData const& data) override {
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
      if (mFlowTable.find(data.mFlowKey) == mFlowTable.end()) {
        mFlowTable.insert_or_assign(data.mFlowKey, std::vector<hd_packet>());
      }
      std::vector<hd_packet> _packetList;
      try {
        _packetList = this->mFlowTable.at(data.mFlowKey);
      } catch (const std::out_of_range& e) {
        std::cerr << "报错位置在: " << __LINE__ << std::endl;
      }
      if (_packetList.empty()) goto merge_into_existing_flow;
      if (_packetList.back().ts_sec - data.mPcapHead.ts_sec >= opt.interval or _packetList.size() == opt.max_packets) {
        if(_packetList.size() >= opt.min_packets) {
          hd_flow flow{data.mFlowKey, std::move(_packetList)};
          std::string _jsonStr;
          struct_json::to_json(flow, _jsonStr); // 序列化
          hd_info(_jsonStr);
          this->append(_jsonStr);
        }
        mFlowTable.erase(data.mFlowKey);
      }
merge_into_existing_flow:
      try {
        this->mFlowTable[data.mFlowKey].emplace_back(packet);
      } catch (const std::out_of_range& e) {
        std::cerr << "报错位置在: " << __LINE__ << std::endl;
      }
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

private:
  SyncedStream<std::fstream> mOutFile;
  //! not thread safe
  std::map<std::string, std::vector<entity::hd_packet>> mFlowTable;
  std::mutex mAccessToFlowTable;
};

} // entity

#endif //HOUND_JSON_FILE_SINK_HPP

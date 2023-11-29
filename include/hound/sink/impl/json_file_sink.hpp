//
// Created by brian on 11/22/23.
//

#ifndef HOUND_JSON_FILE_SINK_HPP
#define HOUND_JSON_FILE_SINK_HPP

#include <hound/sink/base_sink.hpp>
#include <hound/type/synced_stream.hpp>

namespace hd::type {
namespace fs = std::filesystem;
class JsonFileSink : public BaseSink {
  SyncedStream<std::ofstream> mOutFile;
public:
  JsonFileSink(std::string const& fileName) :
      mOutFile(fileName, std::ios::app) {
    auto parent = absolute(fs::path(fileName)).parent_path();
    if (not fs::exists(parent)) {
      fs::create_directories(parent);
    }
    bool isGood = mOutFile.invoke([](std::ofstream& stream) {
      return stream.good();
    });

    if (not isGood) {
      hd_info(RED("无法打开输出文件: "), fileName);
      exit(EXIT_FAILURE);
    }
  }

  /// 写入json文件
  void consumeData(ParsedData const& data) override {
    // std::fprintf(stdout, "%s\n", data.byte_arr);
    return;
  }
};

} // entity

#endif //HOUND_JSON_FILE_SINK_HPP

//
// Created by brian on 11/22/23.
//

#ifndef HOUND_KAFKA_HPP
#define HOUND_KAFKA_HPP

#include <hound/sink/impl/json_file_sink.hpp>

namespace hd::type {

class KafkaSink : public JsonFileSink {

public:
  KafkaSink(std::string const& fileName) : JsonFileSink(fileName) {

  }

  /// 写入发送到kafka
  void consumeData(ParsedData const& data) override {
    // std::fprintf(stdout, "%s\n", data.byte_arr);
    return;
  }
};

} // entity

#endif //HOUND_KAFKA_HPP

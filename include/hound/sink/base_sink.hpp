//
// Created by brian on 11/28/23.
//

#ifndef HOUND_BASE_SINK_HPP
#define HOUND_BASE_SINK_HPP

#include <hound/type/parsed_data.hpp>
#include <hound/common/core-util.hpp>
#include <hound/common/global.hpp>
#include <hound/type/synced_stream.hpp>

namespace hd::type {
/**
 * 默认流量处理：打印控制台
 */
class BaseSink {
  SyncedStream<std::ostream&> mConsole;
public:
  BaseSink() : mConsole(std::cout) {}

  virtual void consumeData(ParsedData const& data) {
    if (not data.HasContent) return;
    std::string buffer;
    this->fillCsvBuffer(data, buffer, '\0');
#if defined(HD_DEV)
    hd_info_one(std::move(buffer));
#else
    mConsole << std::move(buffer) << '\n';
#endif

#if defined(BENCHMARK)
    ++global::num_processed_packet;
#endif//-#if defined(BENCHMARK)
  };

  virtual ~BaseSink() {};

protected:
  void fillCsvBuffer(ParsedData const& data, std::string& buffer, char _back) {
    using namespace global;
    buffer.append(data.m5Tuple).append(",");
    if (opt.caplen) buffer.append(data.mCapLen).append(",");
    if (opt.timestamp) buffer.append(data.mTimestamp).append(",");
    hd::core::processByteArray<IP4_PADSIZE>(opt.include_ip4, data.mIPv4Head, buffer);
    hd::core::processByteArray<TCP_PADSIZE>(opt.include_tcp, data.mTcpHead, buffer);
    hd::core::processByteArray<UDP_PADSIZE>(opt.include_udp, data.mUdpHead, buffer);
    hd::core::processByteArray<>(opt.payload_len > 0, data.mPayload, buffer);
    buffer.back() = _back;
  }
};
} // entity

#endif //HOUND_BASE_SINK_HPP

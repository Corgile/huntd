//
// Created by brian on 11/28/23.
//

#ifndef HOUND_BASE_SINK_HPP
#define HOUND_BASE_SINK_HPP

#include <hound/type/parsed_data.hpp>
#include <hound/common/core.hpp>
#include <hound/common/global.hpp>
#include <hound/type/synced_stream.hpp>

namespace hd::type {
/**
 * 默认流量处理：打印控制台
 */
class BaseSink {
  SyncedStream<std::ostream&> mConsole;

public:
  BaseSink(std::string const&) : mConsole(std::cout) {
  }

  BaseSink() : mConsole(std::cout) {
  }

  virtual void consumeData(ParsedData const& data) {
    // TODO: 异步
    if (not data.HasContent) return;
    std::string buffer;
    this->fillCsvBuffer(data, buffer);
#if defined(HD_DEV)
    hd_line(std::move(buffer));
#else
    mConsole << std::move(buffer);
#endif
  }

  virtual ~BaseSink() = default;

protected:
  void fillCsvBuffer(ParsedData const& data, std::string& buffer) const {
    using namespace global;
    buffer.append(data.m5Tuple).append(",");
    if (opt.caplen) buffer.append(data.mCapLen).append(",");
    if (opt.timestamp) buffer.append(data.mTimestamp).append(",");
    fillRawBitVec(data, buffer);
  }

  void fillRawBitVec(ParsedData const& data, std::string& buffer) const {
    using namespace global;
    core::util::fill<IP4_PADSIZE>(opt.include_ip4, data.mIP4Head, buffer);
    core::util::fill<TCP_PADSIZE>(opt.include_tcp, data.mTcpHead, buffer);
    core::util::fill<UDP_PADSIZE>(opt.include_udp, data.mUdpHead, buffer);
    core::util::fill(opt.payload > 0, data.mPayload, buffer);
    buffer.pop_back();
  }
};
} // entity

#endif //HOUND_BASE_SINK_HPP

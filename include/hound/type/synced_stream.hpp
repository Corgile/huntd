//
// Created by brian on 11/29/23.
//

#ifndef HOUND_SYNCED_STREAM_HPP
#define HOUND_SYNCED_STREAM_HPP

#include <mutex>

namespace hd {
namespace type {
template<typename StreamType>
class SyncedStream {
public:
  SyncedStream(const std::string& filename, std::ios_base::openmode mode)
      : mOutStream(filename, mode) {
  }

  template<typename... Args>
  SyncedStream(const std::string& filename, std::ios_base::openmode mode, Args&& ... args) {
    mOutStream = StreamType(filename, mode, std::forward<Args>(args)...);
  }

  SyncedStream(StreamType&& stream)
      : mOutStream(std::forward<StreamType>(stream)), mutex_() {}

  /// 同步访问流对象的成员函数
  template<typename Func, typename... Args>
  auto SyncInvoke(Func&& func, Args&& ... args) {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::invoke(std::forward<Func>(func), mOutStream, std::forward<Args>(args)...);
  }

  template<typename Func, typename... Args>
  auto invoke(Func&& func, Args&& ... args) {
    return std::invoke(std::forward<Func>(func), mOutStream, std::forward<Args>(args)...);
  }

  template<typename T>
  SyncedStream& operator<<(const T& data) {
    std::scoped_lock<std::mutex> lock(mutex_);
    mOutStream << data;
    return *this;
  }

  /// 特化版本处理操纵符，处理 std::endl
  SyncedStream& operator<<(std::ostream& (* manipulator)(std::ostream&)) {
    std::scoped_lock<std::mutex> lock(mutex_);
    manipulator(mOutStream);
    return *this;
  }

private:
  StreamType mOutStream;
  mutable std::mutex mutex_;
};

} // type
} // hd

#endif //HOUND_SYNCED_STREAM_HPP

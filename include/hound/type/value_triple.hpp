//
// Created by brian on 11/29/23.
//

#ifndef HOUND_VALUE_TRIPLE_HPP
#define HOUND_VALUE_TRIPLE_HPP

namespace hd {
namespace type {
struct ValueTriple {
  __time_t ts_sec;
  __suseconds_t ts_usec;
  bpf_u_int32 caplen;

  ValueTriple() = default;

  ValueTriple(__time_t tsSec, __suseconds_t tsUsec, bpf_u_int32 capLen)
      : ts_sec(tsSec), ts_usec(tsUsec), caplen(capLen) {}
};

} // type
} // hd

#endif //HOUND_VALUE_TRIPLE_HPP

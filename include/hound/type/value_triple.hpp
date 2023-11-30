//
// Created by brian on 11/29/23.
//

#ifndef HOUND_VALUE_TRIPLE_HPP
#define HOUND_VALUE_TRIPLE_HPP

namespace hd {
namespace type {
struct ValueTriple {
  uint32_t ts_sec;
  uint32_t ts_usec;
  uint32_t caplen;

  ValueTriple() = default;

  ValueTriple(uint32_t tsSec, uint32_t tsUsec, uint32_t capLen) : ts_sec(tsSec), ts_usec(tsUsec), caplen(capLen) {}
};

} // type
} // hd

#endif //HOUND_VALUE_TRIPLE_HPP

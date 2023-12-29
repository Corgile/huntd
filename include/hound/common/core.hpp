//
// Created by brian on 12/27/23.
//

#ifndef HOUND_CORE_FUNC_HPP
#define HOUND_CORE_FUNC_HPP

#include <string>
#include <hound/type/byte_array.hpp>
#include <hound/common/global.hpp>
#include <hound/type/stride_t.hpp>

#ifndef BIT
#define BIT(x) ((x) << 3)
#endif // BIT


namespace hd::core {
using namespace hd::type;
using namespace hd::global;

class util {
public:
  template <int32_t PadSize = 0>
  static void fill(bool const condition, const ByteArray& rawData, std::string& buffer) {
    if (not condition) return;
    if constexpr (PadSize == 0) { // payload
      __fill(opt.stride, opt.payload, rawData, buffer);
    } else __fill(opt.stride, PadSize, rawData, buffer);
  }

private:
  static constexpr uint64_t log2(int v) {
    int n = 0;
    while (v > 1) {
      v >>= 1;
      n++;
    }
    return n;
  }

  template <int width>
  static constexpr uint64_t get_buff() {
    uint64_t buff = 1;
    for (int i = 0; i < width - 1; ++i) {
      buff <<= 1;
      buff += 1;
    }
    // buff <<= 64 - width;
    return buff;
  }

  template <int width>
  static void ___fill(int const _wantedFileds, const ByteArray& raw, std::string& refout) {
    int i = 0;
    uint64_t const* p = reinterpret_cast<uint64_t*>(raw.data);
    constexpr uint64_t n = log2(width);
    const int _availableFields = raw.byteLen << 3 >> n;
    constexpr uint64_t b = get_buff<width>();
    constexpr uint64_t r = (64 >> n) - 1;
    constexpr uint64_t s = log2(64 >> n);

    char buffer[22];
    for (; i < _availableFields; ++i) {
      const uint64_t w = (i & r) << n;
      const uint64_t _val = (b << w & p[i >> s]) >> w;//45 00   05 dc a9 93   20 00
      std::sprintf(buffer, "%ld,", _val);
      refout.append(buffer);
    }
    for (; i < _wantedFileds << 3 >> n; ++i) {
      refout.append(fillBit);
    }
  }

  static void __fill(const int stride, const int PadSize, const ByteArray& rawData, std::string& buffer) {
    switch (stride) {
    case 1: ___fill<1>(PadSize, rawData, buffer);
      break;
    case 2: ___fill<2>(PadSize, rawData, buffer);
      break;
    case 4: ___fill<4>(PadSize, rawData, buffer);
      break;
    case 8: ___fill<8>(PadSize, rawData, buffer);
      break;
    case 16: ___fill<16>(PadSize, rawData, buffer);
      break;
    case 32: ___fill<32>(PadSize, rawData, buffer);
      break;
    case 64: ___fill<64>(PadSize, rawData, buffer);
    default: /* handle unsupported stride sizes */break;
    }
  }
};
}// namespace hd::core

#endif // HOUND_CORE_FUNC_HPP

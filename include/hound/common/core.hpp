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
  static void ProcessByteArray(bool const condition, const ByteArray& rawData, std::string& buffer) {
    if (not condition) return;
    if constexpr (PadSize == 0) { // payload
      ProcessStrideDynamic(opt.stride, opt.payload, rawData, buffer);
    } else ProcessStrideDynamic(opt.stride, PadSize, rawData, buffer);
  }

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
  static void ___fill(int const _padSize, const ByteArray& raw, std::string& refout) {
    int i = 0;
    uint64_t const* p = reinterpret_cast<uint64_t*>(raw.data);
    const int bitSize = raw.byteLen << 3;
    constexpr uint64_t n = log2(width);
    constexpr uint64_t b = get_buff<width>();
    constexpr uint64_t r = (64 >> n) - 1;
    constexpr uint64_t s = log2(64 >> n);
    char buffer[22];
    for (; i < bitSize >> n; ++i) {
      const uint64_t w = (i & r) << n;
      const uint64_t ff = b << w;
      const uint64_t byteVal = (ff & p[i >> s]) >> w;//45 00   05 dc a9 93   20 00
      std::sprintf(buffer, "%d,", (int)byteVal);
      refout.append(buffer);
    }
    for (; i < _padSize << 3 >> n; ++i) {
      refout.append(fillBit);
    }
  }

  static void __fill(const int stride, const int PadSize, const ByteArray& rawData, std::string& buffer) {
    switch (stride) {
    case 1: ___fill<1>(PadSize, rawData, buffer);
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

  static void ProcessStrideDynamic(const int stride, const int PadSize, const ByteArray& rawData, std::string& buffer) {
    switch (stride) {
    case 1: ProcessStride<>(PadSize, rawData, buffer);
      break;
    case 8: ProcessStride<stride_t<8>>(PadSize, rawData, buffer);
      break;
    case 16: ProcessStride<stride_t<16>>(PadSize, rawData, buffer);
      break;
    case 32: ProcessStride<stride_t<32>>(PadSize, rawData, buffer);
      break;
    case 64: ProcessStride<stride_t<64>>(PadSize, rawData, buffer);
    default: /* handle unsupported stride sizes */break;
    }
  }

  template <typename T = stride_t<1>>
  static void ProcessStride(const int PadSize, const ByteArray& raw, std::string& buffer) {
    int i = 0;
    constexpr int bitSize = BIT(sizeof(T));
    int const _numField = raw.byteLen / sizeof(T);
    int const _padField = BIT(PadSize) / bitSize;
    T const* arr = reinterpret_cast<T const*>(raw.data);
    for (; i < _numField; ++i) {
      buffer.append(std::to_string(arr[i].val)).append(",");
    }
    for (; i < _padField; ++i) {
      buffer.append(fillBit);
    }
  }

  static void ConvertToBits(uint32_t const value, std::string& buffer) {
    if (value == 0) {
      buffer.append("0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,");
      return;
    }
    char buff[4];
    for (int i = 31; i >= 0; --i) {
      int const bit = value >> i & 0x1;
      std::snprintf(buff, sizeof(buff), "%d,", bit);
      buffer.append(buff);
    }
  }
};
}// namespace hd::core

#endif // HOUND_CORE_FUNC_HPP

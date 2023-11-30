//
// Created by brian on 11/27/23.
//

#ifndef HOUND_CORE_HPP
#define HOUND_CORE_HPP

#include <hound/type/stride_t.hpp>
#include <hound/type/byte_array.hpp>

namespace hd::core {
using namespace hd::type;
using namespace hd::global;

#ifndef BYTE2BIT
  #define BYTE2BIT(x) ((x) << 3)
#endif//BYTE2BIT

/*

template<int32_t fixedPadSize, int8_t STRIDE>
static void appendHeaderData(std::string& buffer, const ByteArray& rawData) {
  int i = 0;
  int32_t const _numField{BYTE2BIT(rawData.byteLen) / STRIDE};
  int32_t const _paddedField{BYTE2BIT(fixedPadSize) / STRIDE};
  stride_t<STRIDE>* typedData = reinterpret_cast<stride_t<STRIDE>*>(rawData.data);
  if (opt.unsign) {
    for (; i < _numField; ++i) {
      unsigned const v = typedData[i].buffer;
      buffer.append(std::to_string(v)).append(",");
    }
  } else {
    for (; i < _numField; ++i) {
      signed const v = typedData[i].buffer;
      buffer.append(std::to_string(v)).append(",");
    }
  }
  for (; i < _paddedField; ++i) {
    buffer.append(fillBit);
  }
}

template<int32_t fixedPadSize, typename T=stride_t<1>>
static void appendHeaderData(std::string& buffer, const ByteArray& rawData) {
  int i = 0;
  int32_t const _numField{BYTE2BIT(rawData.byteLen)};
  int32_t const _paddedField{BYTE2BIT(fixedPadSize)};
  auto const typedData = reinterpret_cast<T*>(rawData.data);
  for (; i < _numField; ++i) {
    buffer.append(std::to_string(typedData[i].buffer)).append(",");
  }
  for (; i < _paddedField; ++i) {
    buffer.append(fillBit);
  }
}

template<uint8_t STRIDE>
static void appendPayload(std::string& buffer, const ByteArray& payload) {
  int i = 0;
  int32_t const _numField{BYTE2BIT(payload.byteLen) / STRIDE};
  int32_t const _paddedField{BYTE2BIT(opt.payload_len - payload.byteLen) / STRIDE};
  stride_t<STRIDE>* typedData = reinterpret_cast<stride_t<STRIDE>*>(payload.data);
  if (opt.unsign) {
    for (; i < _numField; ++i) {
      uint64_t const v = typedData[i].buffer;
      buffer.append(std::to_string(v)).append(",");
    }
  } else {
    for (; i < _numField; ++i) {
      int64_t const v = typedData[i].buffer;
      buffer.append(std::to_string(v)).append(",");
    }
  }
  for (; i < _paddedField; ++i) {
    buffer.append(fillBit);
  }
}

template<typename T=stride_t<1>>
static void appendPayload(std::string& buffer, const ByteArray& payload) {
  int i = 0;
  int32_t const _numField{BYTE2BIT(payload.byteLen)};
  int32_t const _paddedField{BYTE2BIT(opt.payload_len - payload.byteLen)};
  auto const typedData = reinterpret_cast<T*>(payload.data);
  for (; i < _numField; ++i) {
    buffer.append(std::to_string(typedData[i].buffer)).append(",");
  }
  for (; i < _paddedField; ++i) {
    buffer.append(fillBit);
  }
}

/// 定长填充, IPv4, TCP, UDP头部
template<int32_t fixedPadSize>
static void processByteArray(bool cond, const ByteArray& rawData, std::string& buffer) {
  if (not cond) return;
  switch (opt.stride) {
    case 1:
      appendHeaderData<fixedPadSize>(buffer, rawData);
      break;
    default:
    case 8:
      appendHeaderData<fixedPadSize, 8>(buffer, rawData);
      break;
    case 16:
      appendHeaderData<fixedPadSize, 16>(buffer, rawData);
      break;
    case 32:
      appendHeaderData<fixedPadSize, 32>(buffer, rawData);
      break;
    case 64:
      appendHeaderData<fixedPadSize, 64>(buffer, rawData);
      break;
  }
}

/// 不定长填充,payload
template<bool isPayload = true>
static void processByteArray(bool cond, const ByteArray& payload, std::string& buffer) {
  if (not cond) return;
  using namespace hd::global;
  switch (global::opt.stride) {
    case 1:
      appendPayload<>(buffer, payload);
      break;
    default:
    case 8:
      appendPayload<8>(buffer, payload);
      break;
    case 16:
      appendPayload<16>(buffer, payload);
      break;
    case 32:
      appendPayload<32>(buffer, payload);
      break;
    case 64:
      appendPayload<64>(buffer, payload);
      break;
  }
}
*/

static void ConvertToBits(uint32_t value, std::string& buffer) {
  if (value == 0) {
    buffer.append("0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,");
    return;
  }
  char buff[3];
  for (int i = 31; i >= 0; --i) {
    int bit = (value >> i) & 0x1;
    std::snprintf(buff, sizeof(buff), "%d,", bit);
    buffer.append(buff);
  }
}

static void ProcessStride_1(const int PadSize, const ByteArray& rawData, std::string& buffer) {
  int i = 0;
  int const _numField = BYTE2BIT(rawData.byteLen) / 32;
  int const _padField = BYTE2BIT(PadSize) / 32;
  uint32_t const* arr = reinterpret_cast<uint32_t*>(rawData.data);
  for (; i < _numField; ++i) {
    ConvertToBits(arr[i], buffer);
  }
  for (; i < _padField; ++i) {
    ConvertToBits(opt.fill_bit, buffer);
  }
}

static void ProcessStride_8(const int PadSize, const ByteArray& rawData, std::string& buffer) {
  int i = 0;
  uint8_t* arr = rawData.data;
  int const _numField = rawData.byteLen;
  int const _padField = PadSize;
  for (; i < _numField; ++i) {
    buffer.append(std::to_string(arr[i])).append(",");
  }
  for (; i < _padField; ++i) {
    buffer.append(fillBit);
  }
  return;
}

static void ProcessStride_16(const int PadSize, const ByteArray& rawData, std::string& buffer) {
  int i = 0;
  int32_t const _numField{BYTE2BIT(rawData.byteLen) / 16};
  int32_t const _paddedField{BYTE2BIT(PadSize) / 16};
  uint16_t* arr = reinterpret_cast<uint16_t*>(rawData.data);
  for (; i < _numField; ++i) {
    buffer.append(std::to_string(arr[i])).append(",");
  }
  for (; i < _paddedField; ++i) {
    buffer.append(fillBit);
  }
  return;
}

static void ProcessStride_32(const int PadSize, const ByteArray& rawData, std::string& buffer) {
  int i = 0;
  int const _numField = BYTE2BIT(rawData.byteLen) / 32;
  int const _padField = BYTE2BIT(PadSize) / 32;
  uint32_t const* arr = reinterpret_cast<uint32_t*>(rawData.data);
  for (; i < _numField; ++i) {
    buffer.append(std::to_string(arr[i])).append(",");
  }
  for (; i < _padField; ++i) {
    buffer.append(fillBit);
  }
}

static void ProcessStride_64(const int PadSize, const ByteArray& rawData, std::string& buffer) {
  int i = 0;
  int const _numField = rawData.byteLen / sizeof(uint64_t);
  uint64_t const* arr = reinterpret_cast<uint64_t*>(rawData.data);
  for (; i < _numField; ++i) {
    buffer.append(std::to_string(arr[i])).append(",");
  }
  for (; i < PadSize - _numField; ++i) {
    buffer.append(fillBit);
  }
}

template<int32_t PadSize>
static void ProcessByteArray(bool condition, const ByteArray& rawData, std::string& buffer) {
  if (not condition) return;
  if (opt.stride == 1) ProcessStride_1(PadSize, rawData, buffer);
  if (opt.stride == 8) ProcessStride_8(PadSize, rawData, buffer);
  if (opt.stride == 16) ProcessStride_16(PadSize, rawData, buffer);
  if (opt.stride == 32) ProcessStride_32(PadSize, rawData, buffer);
  if (opt.stride == 64) ProcessStride_64(PadSize, rawData, buffer);
}

static void ProcessByteArray(bool condition, const ByteArray& rawData, std::string& buffer) {
  if (not condition) return;
  if (opt.stride == 1) ProcessStride_1(opt.payload_len, rawData, buffer);
  if (opt.stride == 8) ProcessStride_8(opt.payload_len, rawData, buffer);
  if (opt.stride == 16) ProcessStride_16(opt.payload_len, rawData, buffer);
  if (opt.stride == 32) ProcessStride_32(opt.payload_len, rawData, buffer);
  if (opt.stride == 64) ProcessStride_64(opt.payload_len, rawData, buffer);
}


}//namespace hd::core
#endif //HOUND_CORE_HPP

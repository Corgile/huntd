//
// Created by brian on 11/22/23.
//

#ifndef HOUND_CONSOLE_HPP
#define HOUND_CONSOLE_HPP

#include <atomic>
#include <hound/sink/base_sink.hpp>
#include <hound/common/global.hpp>
#include <hound/common/macro.hpp>

namespace hd::entity {
	using namespace hd::type;
	using namespace hd::entity;

	class Console final : public BaseSink {
		// TODO: 用char[]优化
		std::string buffer;

	public:
		explicit Console(raw_packet_info const& data) : BaseSink(data) {
			buffer.append(m5Tuple).append(",");
		}

		[[nodiscard]] byte_t* consumeData() override {
			if (not HasContent) return nullptr;
			auto const v{std::to_string(global::opt.fill_bit).append(",")};
			if (global::opt.caplen) {
				APPEND_SPRINTF(buffer, "%d,", mCapLen);
			}
			if (global::opt.timestamp) {
				APPEND_SPRINTF(buffer, "%ld,%ld,", mTsSec, mTsMsec);
			}
			if (global::opt.include_ip4) {
				/// ipv4头部取60 B
				const auto typedData = reinterpret_cast<int8_t*>(mIpv4Head.data);
				int i = 0;
				for (; i < mIpv4Head.byteLen; ++i) {
					APPEND_SPRINTF(buffer, "%d,", typedData[i]);
				}
				/// 剩下的用 fill_bit 填充
				for (; i < IPV4_HEADER_FILL_LEN; ++i) {
					buffer.append(v);
				}
			}
			if (global::opt.include_tcp) {
				auto const typedData = reinterpret_cast<int8_t*>(mTchHead.data);
				int i = 0;
				for (; i < mTchHead.byteLen; ++i) {
					APPEND_SPRINTF(buffer, "%d,", typedData[i]);
				}
				for (; i < 60; ++i) {
					buffer.append(v);
				}
			}
			if (global::opt.include_udp) {
				auto const typedData = reinterpret_cast<int8_t*>(mUdpHead.data);
				int i = 0;
				for (; i < mUdpHead.byteLen; ++i) {
					APPEND_SPRINTF(buffer, "%d,", typedData[i]);
				}
				for (; i < 8; ++i) {
					buffer.append(v);
				}
			}
			if (global::opt.payload_len > 0) {
				auto const typedData = reinterpret_cast<int8_t*>(mPayload.data);
				int i = 0;
				for (; i < mPayload.byteLen; ++i) {
					APPEND_SPRINTF(buffer, "%d,", typedData[i]);
				}
				for (; i < global::opt.payload_len; ++i) {
					buffer.append(v);
				}
			}
			buffer.pop_back(); // pop "," at the end.
			// @formatter:off
			#if defined(HD_DEV)
			hd_debug(buffer);
			#else
			std::printf("%s\n", buffer.c_str());
			#endif
			#if defined(BENCHMARK)
			++global::num_processed_packet;
			#endif// @formatter:on
			return nullptr;
		}

		~Console() {
		}
	};
} // entity

#endif //HOUND_CONSOLE_HPP

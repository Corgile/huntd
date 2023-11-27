//
// Created by brian on 11/22/23.
//

#ifndef HOUND_CONSOLE_HPP
#define HOUND_CONSOLE_HPP

#include <atomic>
#include <hound/sink/base_sink.hpp>
#include <hound/common/global.hpp>
#include <hound/common/macro.hpp>
#include <hound/common/core-util.hpp>

namespace hd::entity {
	using namespace hd::type;
	using namespace hd::entity;

	class Console final : public BaseSink {
		// ? TODO: 用char[]优化
		std::string buffer;

	public:
		explicit Console(raw_packet_info const& data) : BaseSink(data) {
			buffer.append(m5Tuple).append(",");
		}

		[[nodiscard]] byte_t* consumeData() override {
			if (not HasContent) return nullptr;
			if (global::opt.caplen) {
				APPEND_SPRINTF(buffer, "%d,", mCapLen);
			}
			if (global::opt.timestamp) {
				APPEND_SPRINTF(buffer, "%ld,%ld,", mTsSec, mTsMsec);
			}
			if (global::opt.include_ip4) {
				hd::core::processByteArray<IPV4_PADDING>(mIpv4Head, buffer);
			}
			if (global::opt.include_tcp) {
				hd::core::processByteArray<TCP_PADDING>(mTchHead, buffer);
			}
			if (global::opt.include_udp) {
				hd::core::processByteArray<UDP_PADDING>(mUdpHead, buffer);
			}
			if (global::opt.payload_len > 0) {
				hd::core::processByteArray<>(mPayload, buffer);
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

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
			using namespace global;
			APPEND_SPRINTF(opt.caplen, buffer, "%d,", mCapLen);
			APPEND_SPRINTF(opt.timestamp, buffer, "%ld,%ld,", mTsSec, mTsMsec);
			hd::core::processByteArray<IP4_PADDING>(opt.include_ip4, mIpv4Head, buffer);
			hd::core::processByteArray<TCP_PADDING>(opt.include_tcp, mTchHead, buffer);
			hd::core::processByteArray<UDP_PADDING>(opt.include_udp, mUdpHead, buffer);
			hd::core::processByteArray<>(opt.payload_len > 0, mPayload, buffer);
			buffer.pop_back(); // pop "," at the end.
			hd_info_one(buffer.c_str());
#if defined(BENCHMARK)
			++num_processed_packet;
#endif
			return nullptr;
		}

		~Console() {
		}
	};
} // entity

#endif //HOUND_CONSOLE_HPP

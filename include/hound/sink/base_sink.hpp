//
// Created by brian on 11/22/23.
//

#ifndef HOUND_BASE_SINK_HPP
#define HOUND_BASE_SINK_HPP

#include <cstring>
#include <dbg.h>
#include <hound/common/type.hpp>
#include <hound/common/global.hpp>
#include <hound/common/macro.hpp>

#define LINUX_COOKED_HEADER_SIZE 16
#define ETHERTYPE_IPV4 ETHERTYPE_IP

namespace hd::entity {
	using namespace hd::type;

	class BaseSink {
		uint32_t mLinkType;

	protected:
		// TODO use raw byte
		/// <源_宿> 五元组
		std::string m5Tuple;
		/// 用作map key的排序五元组
		std::string mFlowKey;

		MyValuePair<in_addr_t> mIpPair;
		MyValuePair<std::string> mPortPair;

		uint64_t mTsSec, mTsMsec;
		int16_t mCapLen;
		ByteArray mIpv4Head, mTchHead, mUdpHead, mPayload;

		std::unique_ptr<uint8_t> theArr;

	public:
		bool HasContent{true};

		virtual ~BaseSink() = default;

		explicit BaseSink(raw_packet_info const& data) {
			this->mCapLen = data.info_hdr->caplen;
			this->mTsSec = data.info_hdr->ts.tv_sec;
			this->mTsMsec = data.info_hdr->ts.tv_usec;
			this->HasContent = processRawPacket(data.byte_arr);
		}

		[[nodiscard]] virtual byte_t* consumeData() = 0;

		void setLinkType(uint32_t const _linkType) {
			this->mLinkType = _linkType;
		}

	private:
		[[nodiscard("do not discard")]]
		bool processRawPacket(byte_t* _byteArr) {
			if (mLinkType == DLT_IEEE802_11_RADIO) {
				hd_debug("DLT_IEEE802_11_RADIO");
				return false;
			}
			if (mLinkType == DLT_LINUX_SLL) {
				hd_debug("DLT_LINUX_SLL");
				_byteArr += LINUX_COOKED_HEADER_SIZE;
			}

			// ReSharper disable once CppTooWideScopeInitStatement
			auto const eth{reinterpret_cast<ether_header*>(_byteArr)};
			if (ntohs(eth->ether_type) == ETHERTYPE_VLAN) {
				_byteArr = &_byteArr[sizeof(ether_header) + sizeof(vlan_header)];
			} else {
				_byteArr = &_byteArr[sizeof(ether_header) - sizeof(uint16_t)];
			}
			// ReSharper disable once CppTooWideScopeInitStatement
			uint16_t const realEtherType = ntohs(reinterpret_cast<uint16_t*>(_byteArr)[0]);
			// TODO: 支持其他 ethtype
			if (realEtherType not_eq ETHERTYPE_IPV4) {
				if (realEtherType == ETHERTYPE_IPV6) {
					hd_debug("ETHERTYPE_IPV6");
				} else
					hd_debug("不是 ETHERTYPE_IPV4/6");
				return false;
			}
			return processIPv4Packet(&_byteArr[2]);
		}

		[[nodiscard("do not discard")]]
		bool processIPv4Packet(byte_t* _ipv4RawBytes) {
			ip const* _ipv4 = reinterpret_cast<ip*>(_ipv4RawBytes);
			auto _ipProtocol{_ipv4->ip_p};
			if (_ipProtocol not_eq IPPROTO_UDP and _ipProtocol not_eq IPPROTO_TCP) {
				hd_debug(_ipProtocol);
				return false;
			}
			this->mIpPair = std::minmax(_ipv4->ip_src.s_addr, _ipv4->ip_dst.s_addr);

			m5Tuple.append(inet_ntoa(_ipv4->ip_src)).append("_")
					.append(inet_ntoa(_ipv4->ip_dst)).append("_");

			mFlowKey.append(inet_ntoa({mIpPair.minVal})).append("_")
					.append(inet_ntoa({mIpPair.maxVal})).append("_");

			int32_t const _ipv4HeaderLen = _ipv4->ip_hl * 4;
			this->mIpv4Head = {_ipv4RawBytes, _ipv4HeaderLen};
			byte_t* ipv4Payload{&_ipv4RawBytes[_ipv4HeaderLen]};
			// TODO 策略
			if (_ipProtocol == IPPROTO_TCP) {
				tcphdr const* _tcp = reinterpret_cast<tcphdr*>(ipv4Payload);
				std::string const sport = std::to_string(ntohs(_tcp->th_sport));
				std::string const dport = std::to_string(ntohs(_tcp->th_dport));
				m5Tuple.append(sport).append("_").append(dport).append("_TCP");
				this->mPortPair = std::minmax(sport, dport);
				mFlowKey.append(mPortPair.minVal).append("_")
						.append(mPortPair.maxVal).append("_TCP");
				int const _tcpHL = _tcp->th_off * 4;
				// tcp head
				this->mTchHead = {ipv4Payload, _tcpHL};
				// 处理payload
				int const _byteLen = std::min(ntohs(_ipv4->ip_len) - _ipv4HeaderLen - _tcpHL, global::opt.payload_len);
				this->mPayload = {&_ipv4RawBytes[_ipv4HeaderLen + _tcpHL], _byteLen};
			}
			if (_ipProtocol == IPPROTO_UDP) {
				udphdr const* _udp = reinterpret_cast<udphdr*>(ipv4Payload);
				std::string const sport = std::to_string(ntohs(_udp->uh_sport));
				std::string const dport = std::to_string(ntohs(_udp->uh_dport));
				m5Tuple.append(sport).append("_").append(dport).append("_UDP");
				this->mPortPair = std::minmax(sport, dport);
				mFlowKey.append(mPortPair.minVal).append("_")
						.append(mPortPair.maxVal).append("_UDP");
				int constexpr _udpHL = 8;
				// udp head
				this->mUdpHead = {ipv4Payload, _udpHL};
				// 处理payload
				int const _byteLen = std::min(ntohs(_ipv4->ip_len) - _ipv4HeaderLen - 8, global::opt.payload_len);
				this->mPayload = {&_ipv4RawBytes[_ipv4HeaderLen + _udpHL], _byteLen};
			}
			return true;
		}
	};
} // hd

#endif //HOUND_BASE_SINK_HPP

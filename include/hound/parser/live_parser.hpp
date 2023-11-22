//
// Created by brian on 11/22/23.
//

#ifndef FC_REFACTORED_LIVE_PARSER_HPP
#define FC_REFACTORED_LIVE_PARSER_HPP

#include <pcap/pcap.h>
#include <mutex>
#include <condition_variable>

#include <hound/common/hd_type.hpp>
#include <hound/sink/console/console.hpp>
#include <hound/sink/csv/csv.hpp>
#include <hound/sink/json/json.hpp>
#include <hound/sink/kafka/kafka.hpp>
#include <hound/entity/lock_free_queue.hpp>


namespace hd::entity {
	using namespace hd::type;

	class LiveParser {
	public:
		explicit LiveParser();

		void startCapture();

		~LiveParser();

	private:
		pcap_t* mHandle{nullptr};
		uint32_t mLinkType{};
		// TODO: use double buffer
		hd::entity::LockFreeQueue<raw_packet_info, 1000> rawPacketQueue;
//		std::mutex mtxRawQueue;                   // rawQueue的互斥锁
//		std::condition_variable cv_producer;      // 生产者条件变量
//		std::condition_variable cv_consumer;      // 消费者条件变量

	private:
		static void livePacketHandler(u_char* user_data, const struct pcap_pkthdr* pkthdr,
																	const u_char* packet);

		[[noreturn]] void consumer_job();

		void processPacket(const raw_packet_info& data);
	};

} // entity

#endif //FC_REFACTORED_LIVE_PARSER_HPP

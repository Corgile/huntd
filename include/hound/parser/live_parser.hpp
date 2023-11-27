//
// Created by brian on 11/22/23.
//

#ifndef FC_REFACTORED_LIVE_PARSER_HPP
#define FC_REFACTORED_LIVE_PARSER_HPP

#include <pcap/pcap.h>
#include <mutex>
#include <condition_variable>

#include <hound/common/type.hpp>
#include <hound/sink/console/console.hpp>
#include <hound/sink/csv/csv.hpp>
#include <hound/sink/json/json.hpp>
#include <hound/sink/kafka/kafka.hpp>
#include <hound/entity/lock_free_queue.hpp>
#include <atomic>

namespace hd::entity {
	using namespace hd::type;

	class LiveParser {
	public:
		explicit LiveParser();

		void startCapture();

		void stopCapture() const;

		~LiveParser();

	private:
		pcap_t* mHandle{nullptr};
		uint32_t mLinkType{};
		hd::entity::LockFreeQueue<raw_packet_info, 2048> lockFreeQueue;
		std::atomic<bool> keepRunning{true};

	private:
		static void liveHandler(u_char*, const struct pcap_pkthdr*, const u_char*);

		void consumer_job();
	};

} // entity

#endif //FC_REFACTORED_LIVE_PARSER_HPP

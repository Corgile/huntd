//
// Created by brian on 11/22/23.
//

#ifndef HOUND_KAFKA_HPP
#define HOUND_KAFKA_HPP

namespace hd::entity {

	class Kafka : public BaseSink {

	public:
		Kafka(raw_packet_info& data) : BaseSink(data) {}

		/// 写入发送到kafka
		[[nodiscard]] byte_t* consumeData() override {
			// std::fprintf(stdout, "%s\n", data.byte_arr);
			return nullptr;
		}
	};

} // entity

#endif //HOUND_KAFKA_HPP

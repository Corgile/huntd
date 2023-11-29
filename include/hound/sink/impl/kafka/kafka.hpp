//
// Created by brian on 11/22/23.
//

#ifndef HOUND_KAFKA_HPP
#define HOUND_KAFKA_HPP

namespace hd::type {

	class Kafka : public ParsedData {

	public:
		Kafka(raw_packet_info& data) : ParsedData(data) {}

		/// 写入发送到kafka
		[[nodiscard]] byte_t* consumeData()  {
			// std::fprintf(stdout, "%s\n", data.byte_arr);
			return nullptr;
		}
	};

} // entity

#endif //HOUND_KAFKA_HPP

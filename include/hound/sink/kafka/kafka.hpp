//
// Created by brian on 11/22/23.
//

#ifndef HOUND_KAFKA_HPP
#define HOUND_KAFKA_HPP

namespace hd::entity {

	template<typename T>
	class Kafka : public T {
		static_assert(std::is_base_of<hd::entity::BaseSink, T>::value, "Template argument must be a BaseSink");

	public:
		/// 写入发送到kafka
		[[nodiscard]] byte_t* consumeData(SomeData data) const override {
			std::fprintf(stdout, "%s\n", data);
			return nullptr;
		}
	};

} // entity

#endif //HOUND_KAFKA_HPP

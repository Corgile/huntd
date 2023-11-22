//
// Created by brian on 11/22/23.
//

#ifndef HOUND_CONSOLE_HPP
#define HOUND_CONSOLE_HPP

#include <hound/sink/base_sink.hpp>

namespace hd::entity {

	template<typename T>
	class Console : public T {
		static_assert(std::is_base_of<hd::entity::BaseSink, T>::value, "Template argument must be a BaseSink");

	public:
		[[nodiscard]] byte_t* consumeData(SomeData data) const override {
			std::fprintf(stdout, "%s\n", data);
			return nullptr;
		}
	};

} // entity

#endif //HOUND_CONSOLE_HPP

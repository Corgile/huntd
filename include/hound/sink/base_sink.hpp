//
// Created by brian on 11/22/23.
//

#ifndef HOUND_BASE_SINK_HPP
#define HOUND_BASE_SINK_HPP

#include <hound/common/hd_type.hpp>
#include <hound/entity/double_buff_queue.hpp>

namespace hd::entity {
	using namespace hd::type;

	class BaseSink {
	public:
		[[nodiscard]] virtual byte_t* consumeData(SomeData data) = 0;
	};
} // hd

#endif //HOUND_BASE_SINK_HPP

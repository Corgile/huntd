//
// Created by brian on 11/22/23.
//

#ifndef HOUND_JSON_HPP
#define HOUND_JSON_HPP

namespace hd::entity {
	class Json : public BaseSink {
	public:
		Json(raw_packet_info& data) : BaseSink(data) {}

		/// 写入json文件
		[[nodiscard]] byte_t* consumeData() override {
			// std::fprintf(stdout, "%s\n", data.byte_arr);
			return nullptr;
		}
	};

} // entity

#endif //HOUND_JSON_HPP

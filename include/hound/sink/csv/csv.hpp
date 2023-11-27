//
// Created by brian on 11/22/23.
//

#ifndef HOUND_CSV_HPP
#define HOUND_CSV_HPP

namespace hd ::entity {
	class CSV : public BaseSink {
	public:
		CSV(raw_packet_info& data) : BaseSink(data) {}

		/// 写入csv文件
		[[nodiscard]] byte_t* consumeData() override {
			// std::fprintf(stdout, "%s\n", data.byte_arr);
			return nullptr;
		}
	};
} // entity

#endif //HOUND_CSV_HPP

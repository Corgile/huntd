//
// Created by brian on 11/22/23.
//

#ifndef HOUND_CSV_HPP
#define HOUND_CSV_HPP

namespace hd ::entity {
	template<typename T>
	class CSV : public T {
		static_assert(std::is_base_of<hd::entity::BaseSink, T>::value, "Template argument must be a BaseSink");

	public:
		/// 写入csv文件
		[[nodiscard]] byte_t* consumeData(SomeData data) const override {
			std::fprintf(stdout, "%s\n", data);
			return nullptr;
		}
	};
} // entity

#endif //HOUND_CSV_HPP

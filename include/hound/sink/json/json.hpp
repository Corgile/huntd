//
// Created by brian on 11/22/23.
//

#ifndef HOUND_JSON_HPP
#define HOUND_JSON_HPP

namespace hd::entity {
	template<typename T>
	class Json : public T {
		static_assert(std::is_base_of<hd::entity::BaseSink, T>::value, "Template argument must be a BaseSink");

	public:
		/// 写入json文件
		[[nodiscard]] byte_t* consumeData(SomeData data) const override {
			std::fprintf(stdout, "%s\n", data);
			return nullptr;
		}
	};

} // entity

#endif //HOUND_JSON_HPP

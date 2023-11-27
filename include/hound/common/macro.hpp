//
// Created by brian on 11/22/23.
//

#ifndef HOUND_MACROS_HPP
#define HOUND_MACROS_HPP

#include <iostream>

namespace hd::macro {
	template<typename... T>
	static void printL(T... args) {
		((std::cout << args), ...) << "\n";
	}
} // namespace hd::macro

#pragma region 常量宏 @formatter:off

#if not defined(IPV4_PADDING)
	#define IPV4_PADDING  60
#endif//IPV4_HEADER_PADDING_LEN

#if not defined(TCP_PADDING)
	#define TCP_PADDING  60
#endif//TCP_HEADER_PADDING_LEN

#if not defined(UDP_PADDING)
	#define UDP_PADDING  8
#endif//UDP_HEADER_PADDING_LEN
#pragma endregion 常量宏 @formatter:on

#pragma region 功能性宏 @formatter:off

#if not defined(APPEND_SPRINTF)
	#define APPEND_SPRINTF(buffer, format, ...) 			\
	do { 																							\
		std::unique_ptr<char> const buff(new char[20]);	\
		std::sprintf(buff.get(), format, __VA_ARGS__); 	\
		buffer.append(buff.get()); 											\
	} while (false)
#endif//APPEND_SPRINTF

#if not defined(HD_ANSI_COLOR)
	#define HD_ANSI_COLOR
	#define RED(x)     "\033[31;1m" x "\033[0m"
	#define GREEN(x)   "\033[32;1m" x "\033[0m"
	#define YELLOW(x)  "\033[33;1m" x "\033[0m"
	#define BLUE(x)    "\033[34;1m" x "\033[0m"
	#define CYAN(x)    "\033[36;1m" x "\033[0m"
#endif //HD_ANSI_COLOR

#if not defined(hd_info)
	#define hd_info(x...)     \
	do {                      \
		hd::macro::printL(x);   \
	} while (false)
#endif//INFO

/// 只针对打印一个变量
#if not defined(hd_info_one)
	#define hd_info_one(x)  dbg(x)
#endif//hd_info_one

/// 仅在开发阶段作为调试使用
#if not defined(hd_debug)
	#if defined(HD_DEV)
		#define hd_debug(x)  dbg(x)
	#else//not HD_DEV
		#define hd_debug(x)
	#endif
#endif//hd_debug

#ifndef BYTE2BIT
#define BYTE2BIT(x) ((x) << 3)
#endif//BYTE2BIT
#pragma endregion 功能性宏 @formatter:on

#endif //HOUND_MACROS_HPP

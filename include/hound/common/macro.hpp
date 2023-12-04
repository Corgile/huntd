//
// Created by brian on 11/22/23.
//

#ifndef HOUND_MACROS_HPP
#define HOUND_MACROS_HPP

#include <dbg.h>
#include <iostream>

namespace hd::macro {
template<typename... T>
static void printL(T... args) {
  ((std::cout << args), ...) << "\n";
}
} // namespace hd::macro

#pragma region 常量宏 @formatter:off

#if not defined(CONSOLE)
/// print to console
#define CONSOLE	 0x1
#endif

#if not defined(TEXT_FILE)
/// write to csv/ascii text file
#define TEXT_FILE	 0x2
#endif

#if not defined(JSON_FILE)
/// write to json file
#define JSON_FILE	 0x4
#endif

#if not defined(MSG_QUEUE)
/// send to some message queue (kafka)
#define MSG_QUEUE	 0x8
#endif

#if not defined(SILENT)
/// silent
#define SILENT	 -1
#endif

#if not defined(IP4_PADSIZE)
#define IP4_PADSIZE  60
#endif//IPV4_HEADER_PADDING_LEN

#if not defined(TCP_PADSIZE)
#define TCP_PADSIZE  60
#endif//TCP_HEADER_PADDING_LEN

#if not defined(UDP_PADSIZE)
#define UDP_PADSIZE  8
#endif//UDP_HEADER_PADDING_LEN
#pragma endregion 常量宏 @formatter:on

#pragma region 功能性宏 @formatter:off

#if not defined(APPEND_SPRINTF)
#define APPEND_SPRINTF(cond, buffer, format, ...) 			\
	do { 																							\
		if(not cond) break;															\
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
#if defined(HD_DEV)
#define hd_info_one(x)  dbg(x)
#else
		#define hd_info_one(x)  std::cout << x << "\n";
#endif
#endif//-hd_info_one

/// 仅在开发阶段作为调试使用
#if not defined(hd_debug)
#if defined(HD_DEV)
#define hd_debug  dbg
#else//- not HD_DEV
		#define hd_debug(...)
#endif
#endif//- hd_debug

#pragma endregion 功能性宏 @formatter:on

#endif //HOUND_MACROS_HPP

//
// Created by brian on 11/22/23.
//

#ifndef FC_REFACTORED_FC_MACROS_HPP
#define FC_REFACTORED_FC_MACROS_HPP

#include <iostream>
#include <hound/common/hd_util.hpp>

namespace hd::macro {
	template<typename... T>
	static void printL(T... args) {
		((std::cout << args), ...) << std::endl;
	}
}

#pragma region 宏
#ifndef LOG
#if defined(DEBUG_SCOPE)
#define LOG(x...)           \
	do {                      \
		hd::printL(x);         \
	} while (0)
#else
#define LOG(x...)
#endif
#endif

#ifndef RED
#define RED(x) "\033[31;1m" x "\033[0m"
#endif

#ifndef GREEN
#define GREEN(x) "\033[32;1m" x "\033[0m"
#endif

#ifndef YELLOW
#define YELLOW(x) "\033[33;1m" x "\033[0m"
#endif

#ifndef BLUE
#define BLUE(x) "\033[34;1m" x "\033[0m"
#endif

#ifndef CYAN
#define CYAN(x) "\033[36;1m" x "\033[0m"
#endif

#ifndef INFO
#define INFO(x...)          \
  do {                      \
    hd::macro::printL(x);    \
  } while (0)
#endif

#ifndef BYTE2BIT
#define BYTE2BIT(x) ((x) << 3)
#endif
#pragma endregion

#endif //FC_REFACTORED_FC_MACROS_HPP

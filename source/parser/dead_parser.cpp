//
// Created by brian on 11/22/23.
//

#include <hound/parser/dead_parser.hpp>

#if defined(BENCHMARK)

#include <hound/entity/timer.hpp>

#endif

void hd::entity::DeadParser::processFile() {
#if defined(BENCHMARK)
	Timer timer;
#endif
}

//
// Created by brian on 11/22/23.
//

#include <hound/parser/dead_parser.hpp>

#if defined(BENCHMARK)

  #include <hound/type/timer.hpp>

#endif

void hd::type::DeadParser::processFile() {
#if defined(BENCHMARK)
  double _timeConsumption = 0;
  Timer timer(_timeConsumption);
#endif
}

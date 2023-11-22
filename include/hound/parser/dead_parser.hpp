//
// Created by brian on 11/22/23.
//

#ifndef FC_REFACTORED_DEAD_PARSER_HPP
#define FC_REFACTORED_DEAD_PARSER_HPP

#include <hound/sink/console/console.hpp>
#include <hound/sink/csv/csv.hpp>
#include <hound/sink/json/json.hpp>

namespace hd::entity {
	class DeadParser {

	public:
		void processFile();
	};
}


#endif //FC_REFACTORED_DEAD_PARSER_HPP

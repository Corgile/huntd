#include <hound/common/hd_util.hpp>
#include <hound/common/hd_global.hpp>
#include <hound/parser/live_parser.hpp>
#include <hound/parser/dead_parser.hpp>

namespace hd::global {
	capture_option opt;
	uint32_t num_captured_packet;
	uint32_t num_missed_packet;
	uint32_t num_consumed_packet;
}

int main(int argc, char* argv[]) {
	using namespace hd::global;
#if defined(BENCHMARK)
	num_captured_packet = 0;
	num_missed_packet = 0;
	num_consumed_packet = 0;
#endif
	hd::util::parse_options(opt, argc, argv);
	opt.print();
	if (opt.live_mode) {
#if defined(LIVE_MODE)
		hd::entity::LiveParser().startCapture();
#endif
	} else {
#if defined(OFFLINE_MODE)
		hd::entity::DeadParser().processFile();
#endif
	}
#if defined(SEND_KAFKA)
#endif
	return 0;
}

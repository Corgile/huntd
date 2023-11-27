#include <csignal>
#include <thread>
#include <hound/common/util.hpp>
#include <hound/common/global.hpp>
#include <hound/parser/live_parser.hpp>
#include <hound/parser/dead_parser.hpp>

namespace hd::global {
	capture_option opt;
#if defined(BENCHMARK)
	uint32_t num_captured_packet = 0;
	std::atomic<int32_t> num_missed_packet = 0;
	std::atomic<int32_t> num_consumed_packet = 0;
	std::atomic<int32_t> num_processed_packet = 0;
#endif
}

int main(int argc, char* argv[]) {
	using namespace hd::global;
	hd::util::parse_options(opt, argc, argv);
	opt.print();
	static std::unique_ptr<LiveParser> liveParser;

	std::signal(SIGINT | SIGTERM | SIGKILL | SIGABRT, [](int signal) {
		if (signal == SIGINT) {
			std::cout << "\n\033[31;1m[Ctrl-C]\033[0m received. 正在结束..." << std::endl;
			liveParser->stopCapture();
		}
		if (signal == SIGTERM) {
			std::cout << "\n\033[31;1m[SIGTERM]\033[0m received. 正在结束..." << std::endl;
			liveParser->stopCapture();
		}
		if (signal == SIGKILL) {
			std::cout << "\n\033[31;1m[SIGKILL]\033[0m received. 正在结束..." << std::endl;
			liveParser->stopCapture();
		}
		if (signal == SIGABRT) {
			std::cout << "\n\033[31;1m[SIGKILL]\033[0m received. 正在结束..." << std::endl;
			liveParser->stopCapture();
		}
	});

	if (opt.live_mode) {
#if defined(LIVE_MODE)
		liveParser = std::make_unique<LiveParser>();
		std::thread([&]() {
			liveParser->startCapture();
		}).join();
#endif
	} else {
#if defined(DEAD_MODE)
		hd::entity::DeadParser().processFile();
#endif
	}
#if defined(INCLUDE_KAFKA)
	std::cout << "INCLUDE_KAFKA" << std::endl;
#endif
	return 0;
}

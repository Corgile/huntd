#include <csignal>
#include <hound/common/util.hpp>
#include <hound/common/global.hpp>

//@formatter:off
#if defined(LIVE_MODE)
  #include <thread>
  #include <hound/parser/live_parser.hpp>
#endif
#if defined(DEAD_MODE)
  #include <hound/parser/dead_parser.hpp>
#endif
//@formatter:on
namespace hd::global {
hd::type::capture_option opt;
std::string fillBit;
#if defined(BENCHMARK)
uint32_t num_captured_packet = 0;
std::atomic<int32_t> num_missed_packet = 0;
std::atomic<int32_t> num_consumed_packet = 0;
std::atomic<int32_t> num_processed_packet = 0;
#endif
}

int main(int argc, char* argv[]) {
  using namespace hd::global;
  hd::util::parseOptions(opt, argc, argv);
  if (opt.unsign or opt.stride == 1) opt.fill_bit |= 0;
  fillBit = std::to_string(opt.fill_bit).append(",");
#if defined(LIVE_MODE)
  static std::unique_ptr<hd::type::LiveParser> liveParser{nullptr};
#endif
#if defined(DEAD_MODE)
  static std::unique_ptr<hd::type::DeadParser> deadParser{nullptr};
#endif
  std::signal(SIGINT | SIGTERM | SIGKILL | SIGSEGV, [](int signal) {
    if (signal == SIGINT) {
      hd_info("\n\033[31;1m[Ctrl-C] \033[0m received. 即将退出...");
    }
    if (signal == SIGTERM) {
      hd_info("\n\033[31;1m[SIGTERM] \033[0m received. 即将退出...");
    }
    if (signal == SIGKILL) {
      hd_info("\n\033[31;1m[SIGKILL] \033[0m received. 即将退出...");
    }
    if (signal == SIGSEGV) {
      hd_info(RED("发生了一个段错误: Invalid access to storage."));
    }
  #if defined(LIVE_MODE)
    liveParser->stopCapture();
  #endif
  #if defined(DEAD_MODE)
    // deadParser->stopProcess();
  #endif
  });

#if defined(LIVE_MODE)
  liveParser = std::make_unique<hd::type::LiveParser>();
  if (opt.live_mode) {
    std::thread([&]() {
      liveParser->startCapture();
    }).join();
  } else
#endif
  {
#if defined(DEAD_MODE)
    deadParser = std::make_unique<hd::type::DeadParser>();
    deadParser->processFile();
#endif
  }
  #if defined(INCLUDE_KAFKA)
  std::cout << "INCLUDE_KAFKA" << std::endl;
  #endif
  return 0;
}

#include <csignal>
#include <hound/common/util.hpp>
#include <hound/common/global.hpp>

//@formatter:off
#if defined(LIVE_MODE)
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
std::atomic<int32_t> num_captured_packet = 0;
std::atomic<int32_t> num_dropped_packets = 0;
std::atomic<int32_t> num_consumed_packet = 0;
std::atomic<int32_t> num_written_csv = 0;
#endif
}

int main(int argc, char *argv[]) {
  using namespace hd::global;
  using namespace hd::type;
  hd::util::ParseOptions(opt, argc, argv);
  if (opt.unsign or opt.stride == 1) opt.fill_bit |= 0;
  fillBit = std::to_string(opt.fill_bit).append(",");
#if defined(LIVE_MODE)
  static std::unique_ptr<hd::type::LiveParser> liveParser{nullptr};
#endif
#if defined(DEAD_MODE)
  static std::unique_ptr<hd::type::DeadParser> deadParser{nullptr};
#endif
  static int ctrlc = 0, max__ = 5;
  auto handler = [](int const signal) {
    if (signal == SIGINT) {
      auto const more = max__ - ++ctrlc;
      if (more > 0) {
        hd_line(RED("\n再按 "), more, RED(" 次 [Ctrl-C] 退出"));
      }
      if (ctrlc >= max__) {
        exit(EXIT_FAILURE);
      }
    }
    if (signal == SIGTERM) {
      hd_line(RED("\n[SIGTERM] received. 即将退出..."));
    }
    if (signal == SIGKILL) {
      hd_line(RED("\n[SIGKILL] received. 即将退出..."));
    }
    // if (signal == SIGSEGV) {
    //   hd_line(RED("发生了一个段错误: Invalid access to storage."));
    //   exit(EXIT_FAILURE);
    // }
#if defined(LIVE_MODE)
    liveParser->stopCapture();
#endif
#if defined(DEAD_MODE)
    // deadParser->stopProcess();
#endif
  };
  std::signal(SIGSTOP, handler);
  std::signal(SIGINT, handler);
  std::signal(SIGTERM, handler);
  std::signal(SIGKILL, handler);
  // std::signal(SIGSEGV, handler);
#if defined(LIVE_MODE)
  if (opt.live_mode) {
    liveParser = std::make_unique<LiveParser>();
    liveParser->startCapture();
    liveParser->stopCapture();
  } else
#endif
  {
#if defined(DEAD_MODE)
    deadParser = std::make_unique<DeadParser>();
    deadParser->processFile();
#endif
  }
  return 0;
}

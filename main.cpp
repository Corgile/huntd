#include <csignal>
#include <hound/common/util.hpp>
#include <hound/common/global.hpp>

#include <hound/parser/live_parser.hpp>

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

int main(const int argc, char *argv[]) {
  using namespace hd::global;
  using namespace hd::type;
  hd::util::ParseOptions(opt, argc, argv);
  if (opt.stride == 1) opt.fill_bit |= 0;
  fillBit = std::to_string(opt.fill_bit).append(",");
  static std::unique_ptr<hd::type::LiveParser> liveParser{nullptr};
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
    liveParser->stopCapture();
  };
  std::signal(SIGSTOP, handler);
  std::signal(SIGINT, handler);
  std::signal(SIGTERM, handler);
  std::signal(SIGKILL, handler);
  // std::signal(SIGSEGV, handler);
  liveParser = std::make_unique<LiveParser>();
  liveParser->startCapture();
  liveParser->stopCapture();
  return 0;
}

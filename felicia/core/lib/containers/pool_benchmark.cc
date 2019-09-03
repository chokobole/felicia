#include "felicia/core/lib/containers/pool.h"

#include <queue>

#include "benchmark/benchmark.h"
#include "third_party/chromium/base/compiler_specific.h"

namespace felicia {

namespace {

template <typename T>
class QueueWithSize {
 public:
  explicit constexpr QueueWithSize(uint8_t size) : size_(size) {}

  ALWAYS_INLINE void push(const T& value) {
    while (size_ <= queue_.size()) queue_.pop();
    queue_.push(value);
  }

  ALWAYS_INLINE void push(T&& value) {
    while (size_ <= queue_.size()) queue_.pop();
    queue_.push(std::move(value));
  }

  ALWAYS_INLINE void pop() { queue_.pop(); }

 private:
  uint8_t size_;
  std::queue<T> queue_;
};

}  // namespace

using Uint8Pool = Pool<int, uint8_t>;

template <typename QueueType>
static void BM_Push(benchmark::State& state) {
  int size = state.range(0);
  for (auto _ : state) {
    QueueType pool(10);
    for (int i = 0; i < size; i++) {
      pool.push(i);
    }
  }
}

template <typename QueueType>
static void BM_PushAndPop(benchmark::State& state) {
  int size = state.range(0);
  for (auto _ : state) {
    QueueType pool(10);
    for (int i = 0; i < size; i++) {
      if (i % 2 == 0) {
        pool.push(i);
      } else {
        pool.pop();
      }
    }
  }
}

BENCHMARK_TEMPLATE(BM_Push, Uint8Pool)->Arg(10000);
BENCHMARK_TEMPLATE(BM_Push, QueueWithSize<int>)->Arg(10000);
BENCHMARK_TEMPLATE(BM_PushAndPop, Uint8Pool)->Arg(10000);
BENCHMARK_TEMPLATE(BM_PushAndPop, QueueWithSize<int>)->Arg(10000);

// clang-format off
// 2019-09-03 15:29:19
// Running bazel-bin/felicia/core/lib/pool_benchmark
// Run on (8 X 4900 MHz CPU s)
// CPU Caches:
//   L1 Data 32K (x8)
//   L1 Instruction 32K (x8)
//   L2 Unified 256K (x8)
//   L3 Unified 12288K (x1)
// -------------------------------------------------------------------------------
// Benchmark                                        Time           CPU Iterations
// -------------------------------------------------------------------------------
// BM_Push<Uint8Pool>/10000                     11437 ns      11437 ns      61290
// BM_Push<QueueWithSize<int>>/10000            30471 ns      30469 ns      22466
// BM_PushAndPop<Uint8Pool>/10000                7561 ns       7556 ns      91394
// BM_PushAndPop<QueueWithSize<int>>/10000      14565 ns      14565 ns      47995
// clang-format on

}  // namespace felicia
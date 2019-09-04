#include "felicia/core/lib/containers/pool.h"

#include <queue>

#include "benchmark/benchmark.h"
#include "third_party/chromium/base/compiler_specific.h"
#include "third_party/chromium/base/containers/queue.h"

namespace felicia {

namespace {

template <typename QueueTy>
class QueueWithFixedSize {
 public:
  typedef typename QueueTy::value_type value_type;

  explicit constexpr QueueWithFixedSize(size_t size) : size_(size) {}

  ALWAYS_INLINE void push(const value_type& value) {
    while (size_ <= queue_.size()) queue_.pop();
    queue_.push(value);
  }

  ALWAYS_INLINE void push(value_type&& value) {
    while (size_ <= queue_.size()) queue_.pop();
    queue_.push(std::move(value));
  }

  ALWAYS_INLINE void pop() { queue_.pop(); }

  ALWAYS_INLINE size_t size() { return queue_.size(); }

 private:
  size_t size_;
  QueueTy queue_;
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
BENCHMARK_TEMPLATE(BM_Push, QueueWithFixedSize<std::queue<int>>)->Arg(10000);
BENCHMARK_TEMPLATE(BM_Push, QueueWithFixedSize<base::queue<int>>)->Arg(10000);
BENCHMARK_TEMPLATE(BM_PushAndPop, Uint8Pool)->Arg(10000);
BENCHMARK_TEMPLATE(BM_PushAndPop, QueueWithFixedSize<std::queue<int>>)
    ->Arg(10000);
BENCHMARK_TEMPLATE(BM_PushAndPop, QueueWithFixedSize<base::queue<int>>)
    ->Arg(10000);

// clang-format off
// 2019-09-04 13:08:59
// Running bazel-bin/felicia/core/lib/pool_benchmark
// Run on (8 X 4900 MHz CPU s)
// CPU Caches:
//   L1 Data 32K (x8)
//   L1 Instruction 32K (x8)
//   L2 Unified 256K (x8)
//   L3 Unified 12288K (x1)
// -------------------------------------------------------------------------------------------------
// Benchmark                                                          Time           CPU Iterations
// -------------------------------------------------------------------------------------------------
// BM_Push<Uint8Pool>/10000                                        9347 ns       9347 ns      74087
// BM_Push<QueueWithFixedSize<std::queue<int>>>/10000             29876 ns      29872 ns      22942
// BM_Push<QueueWithFixedSize<base::queue<int>>>/10000            39480 ns      39479 ns      17364
// BM_PushAndPop<Uint8Pool>/10000                                  9499 ns       9498 ns      76497
// BM_PushAndPop<QueueWithFixedSize<std::queue<int>>>/10000       14874 ns      14874 ns      47048
// BM_PushAndPop<QueueWithFixedSize<base::queue<int>>>/10000      17828 ns      17827 ns      38314
// clang-format on

}  // namespace felicia
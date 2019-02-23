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

  ALWAYS_INLINE void pop() { queue_, pop(); }

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
    QueueType pool(255);
    for (int i = 0; i < size; i++) {
      pool.push(i);
    }
  }
}

BENCHMARK_TEMPLATE(BM_Push, Uint8Pool)->Arg(1000);
BENCHMARK_TEMPLATE(BM_Push, QueueWithSize<int>)->Arg(1000);

// --------------------------------------------------------------------------
// Benchmark                                   Time           CPU Iterations
// --------------------------------------------------------------------------
// BM_Push<Uint8Pool>/100000               0 ns          0 ns 1000000000
// BM_Push<QueueWithSize<int>>/100000     264622 ns     264012 ns       2650

}  // namespace felicia
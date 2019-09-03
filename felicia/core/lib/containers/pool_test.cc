#include "felicia/core/lib/containers/pool.h"

#include <stdint.h>

#include "gtest/gtest.h"

namespace felicia {

TEST(PoolTest, Push) {
  Pool<int, uint8_t> pool(2);
  ASSERT_EQ(0, pool.size());
  pool.push(0);  // 0
  ASSERT_EQ(0, pool.front());
  ASSERT_EQ(0, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.push(1);  // 0 1
  ASSERT_EQ(0, pool.front());
  ASSERT_EQ(1, pool.back());
  ASSERT_EQ(2, pool.size());
  pool.pop();  // 1
  ASSERT_EQ(1, pool.front());
  ASSERT_EQ(1, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.pop();  //
  ASSERT_EQ(0, pool.size());
  pool.pop();
}

TEST(PoolTest, Emplace) {
  struct Data {
    Data() = default;
    Data(std::string&& data) : data(data) {}

    std::string data;
  };

  Pool<Data, uint8_t> pool(2);
  pool.emplace("abc");
  ASSERT_EQ("abc", pool.back().data);
  pool.emplace("def");
  ASSERT_EQ("def", pool.back().data);
}

TEST(PoolTest, PushAfterPopAll) {
  Pool<int, uint8_t> pool(2);
  pool.push(0);  // 0
  pool.push(1);  // 0 1
  pool.pop();    // 1
  pool.pop();    //
  pool.pop();    //
  pool.push(2);  // 2
  pool.push(3);  // 2 3
  ASSERT_EQ(2, pool.front());
  ASSERT_EQ(3, pool.back());
  ASSERT_EQ(2, pool.size());
  pool.pop();  // 3
  ASSERT_EQ(3, pool.front());
  ASSERT_EQ(3, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.pop();
  ASSERT_EQ(0, pool.size());
  pool.pop();  //
  pool.pop();
}

TEST(PoolTest, PushWhenPoolIsFull) {
  Pool<int, uint8_t> pool(2);
  pool.push(0);  // 0
  pool.push(1);  // 0 1
  pool.push(2);  // 2 1
  ASSERT_EQ(1, pool.front());
  ASSERT_EQ(2, pool.back());
  ASSERT_EQ(2, pool.size());
  pool.pop();  // 2
  ASSERT_EQ(2, pool.front());
  ASSERT_EQ(2, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.pop();  //
  ASSERT_EQ(0, pool.size());
  pool.pop();
  pool.push(3);  // 3
  ASSERT_EQ(3, pool.front());
  ASSERT_EQ(3, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.pop();  //
  pool.pop();
}

TEST(PoolTest, PushWhenOneSizeIsFull) {
  Pool<int, uint8_t> pool(1);
  pool.push(0);  // 0
  ASSERT_EQ(0, pool.front());
  ASSERT_EQ(0, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.push(1);  // 1
  ASSERT_EQ(1, pool.front());
  ASSERT_EQ(1, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.push(2);  // 2
  ASSERT_EQ(2, pool.front());
  ASSERT_EQ(2, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.pop();    //
  pool.push(3);  // 3
  ASSERT_EQ(3, pool.front());
  ASSERT_EQ(3, pool.back());
  ASSERT_EQ(1, pool.size());
  pool.pop();  //
  pool.pop();
}

TEST(PoolTest, Shrink) {
  Pool<int, uint8_t> pool(4);
  pool.push(0);  // 0
  pool.push(1);  // 0 1
  pool.push(2);  // 0 1 2
  pool.push(3);  // 0 1 2 3
  pool.reserve(2);
  ASSERT_EQ(2, pool.front());
  pool.pop();
  ASSERT_EQ(3, pool.front());
}

TEST(PoolTest, Grow) {
  Pool<int, uint8_t> pool(2);
  pool.push(0);  // 0
  pool.push(1);  // 0 1
  pool.reserve(4);
  pool.push(2);  // 0 1 2
  ASSERT_EQ(0, pool.front());
  pool.pop();
  ASSERT_EQ(1, pool.front());
  pool.pop();
  ASSERT_EQ(2, pool.front());
}

class ABC {
 public:
  ABC() { alive_++; }

  ABC(const ABC& other) { alive_++; }

  ABC(ABC&& other) noexcept { alive_++; }

  ~ABC() { alive_--; }

  static int alive_;
};

int ABC::alive_ = 0;

TEST(PoolTest, AliveTest) {
  felicia::Pool<ABC, uint8_t> pool(10);
  for (int i = 0; i < 20; i++) {
    ABC a;
    pool.push(a);
  }

  ASSERT_EQ(ABC::alive_, 10);

  for (int i = 0; i < 5; i++) {
    pool.pop();
  }

  ASSERT_EQ(ABC::alive_, 5);

  for (int i = 0; i < 20; i++) {
    ABC a;
    pool.push(std::move(a));
  }

  ASSERT_EQ(ABC::alive_, 10);
}

namespace {

template <typename Iterator>
void ExpectForwardIteratorEq(Iterator begin, Iterator end, int from, int to) {
  int n = from;
  for (auto it = begin; it != end; it++) {
    EXPECT_EQ(*it, n++);
  }
  EXPECT_EQ(n, to);
}

}  // namespace

TEST(PoolTest, IteratorTest) {
  Pool<int, uint8_t> pool(10);
  for (int i = 0; i < 5; ++i) {
    pool.push(i);
  }
  ExpectForwardIteratorEq(pool.begin(), pool.end(), 0, 5);
  ExpectForwardIteratorEq(pool.cbegin(), pool.cend(), 0, 5);

  for (int i = 5; i < 10; ++i) {
    pool.push(i);
  }
  ExpectForwardIteratorEq(pool.begin(), pool.end(), 0, 10);
  ExpectForwardIteratorEq(pool.cbegin(), pool.cend(), 0, 10);

  for (int i = 10; i < 15; ++i) {
    pool.push(i);
  }
  ExpectForwardIteratorEq(pool.begin(), pool.end(), 5, 15);
  ExpectForwardIteratorEq(pool.cbegin(), pool.cend(), 5, 15);

  pool.pop();
  pool.pop();
  ExpectForwardIteratorEq(pool.begin(), pool.end(), 7, 15);
  ExpectForwardIteratorEq(pool.cbegin(), pool.cend(), 7, 15);
}

}  // namespace felicia
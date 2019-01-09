#include "felicia/core/lib/containers/fast_pool.h"

#include <stdint.h>

#include "gtest/gtest.h"

namespace felicia {

TEST(FastPoolTest, Push) {
  FastPool<int, uint8_t> pool(2);
  pool.push(0);  // 0
  ASSERT_EQ(0, pool.front());
  ASSERT_EQ(0, pool.back());
  ASSERT_EQ(1, pool.length());
  pool.push(1);  // 0 1
  ASSERT_EQ(0, pool.front());
  ASSERT_EQ(1, pool.back());
  ASSERT_EQ(2, pool.length());
  pool.pop();  // 1
  ASSERT_EQ(1, pool.front());
  ASSERT_EQ(1, pool.back());
  ASSERT_EQ(1, pool.length());
  pool.pop();  //
  ASSERT_EQ(0, pool.length());
  pool.pop();
}

TEST(FastPoolTest, PushWhenPoolIsFull) {
  FastPool<int, uint8_t> pool(2);
  pool.push(0);  // 0
  pool.push(1);  // 0 1
  pool.push(2);  // 2 1
  ASSERT_EQ(1, pool.front());
  ASSERT_EQ(2, pool.back());
  ASSERT_EQ(2, pool.length());
  pool.pop();  // 2
  ASSERT_EQ(2, pool.front());
  ASSERT_EQ(2, pool.back());
  ASSERT_EQ(1, pool.length());
  pool.pop();  //
  ASSERT_EQ(0, pool.length());
  pool.pop();
  pool.push(3);  // 3
  ASSERT_EQ(3, pool.front());
  ASSERT_EQ(3, pool.back());
  ASSERT_EQ(1, pool.length());
  pool.pop();  //
  pool.pop();
}

TEST(FastPoolTest, PushWhenOneSizeIsFull) {
  FastPool<int, uint8_t> pool(1);
  pool.push(0);  // 0
  ASSERT_EQ(0, pool.front());
  ASSERT_EQ(0, pool.back());
  ASSERT_EQ(1, pool.length());
  pool.push(1);  // 1
  ASSERT_EQ(1, pool.front());
  ASSERT_EQ(1, pool.back());
  ASSERT_EQ(1, pool.length());
  pool.push(2);  // 2
  ASSERT_EQ(2, pool.front());
  ASSERT_EQ(2, pool.back());
  ASSERT_EQ(1, pool.length());
  pool.pop();    //
  pool.push(3);  // 3
  ASSERT_EQ(3, pool.front());
  ASSERT_EQ(3, pool.back());
  ASSERT_EQ(1, pool.length());
  pool.pop();  //
  pool.pop();
}

}  // namespace felicia
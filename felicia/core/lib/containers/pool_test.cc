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

}  // namespace felicia
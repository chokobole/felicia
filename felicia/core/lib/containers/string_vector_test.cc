#include "felicia/core/lib/containers/string_vector.h"

#include "gtest/gtest.h"

#include "third_party/chromium/base/stl_util.h"
#include "third_party/chromium/base/test/move_only_int.h"

#include "felicia/core/lib/unit/geometry/point.h"

namespace felicia {

TEST(StringVectorTest, PushAndPopTest) {
  StringVector vector;
  Pointi p(1, 2);
  Pointi p2(3, 4);
  vector.push_back(p);
  vector.push_back(p2);
  EXPECT_EQ(vector.size<Pointi>(), 2);

  Pointi& p3 = vector.at<Pointi>(0);
  EXPECT_EQ(p, p3);
  EXPECT_EQ(p, vector.front<Pointi>());
  Pointi& p4 = vector.at<Pointi>(1);
  EXPECT_EQ(p2, p4);
  EXPECT_EQ(p2, vector.back<Pointi>());

  p3.set_x(5);
  EXPECT_EQ(p3.x(), 5);
  EXPECT_EQ(vector.at<Pointi>(0).x(), 5);

  vector.pop_back<Pointi>();
  EXPECT_EQ(1, vector.size<Pointi>());
  EXPECT_EQ(vector.front<Pointi>(), vector.back<Pointi>());
}

namespace {

template <typename Iterator>
void ExpectForwardEq(Iterator begin, Iterator end, int from, int to) {
  int n = from;
  bool incremental = to > from;
  for (auto it = begin; it != end; it++) {
    if (incremental) {
      EXPECT_EQ(*it, n++);
    } else {
      EXPECT_EQ(*it, n--);
    }
  }
  EXPECT_EQ(n, to);
}

}  // namespace

TEST(StringVectorTest, IteratorTest) {
  StringVector vector;
  for (int i = 0; i < 10; ++i) {
    vector.push_back<int>(i);
  }

  StringVector::Iterator<int> iterator = vector.Iterated<int>();
  ExpectForwardEq(iterator.begin(), iterator.end(), 0, 10);
  ExpectForwardEq(iterator.cbegin(), iterator.cend(), 0, 10);
  ExpectForwardEq(iterator.rbegin(), iterator.rend(), 9, -1);
  ExpectForwardEq(iterator.rbegin(), iterator.rend(), 9, -1);
}

TEST(StringVectorTest, InsertTest) {
  StringVector vector;
  StringVector::Iterator<int> iterator = vector.Iterated<int>();
  auto it = vector.insert(iterator.begin(), 0);
  it = vector.insert(it, 1);
  iterator = vector.Iterated<int>();
  it = vector.insert(iterator.begin() + 1, 2);
  it = vector.insert(it, 3);
  vector.insert(it, 3, 4);
  vector.insert(vector.Iterated<int>().end(), 5);
  vector.push_back(6);

  int answer[] = {1, 4, 4, 4, 3, 2, 0, 5, 6};
  for (size_t i = 0; i < base::size(answer); ++i) {
    EXPECT_EQ(answer[i], vector.at<int>(i));
  }
}

TEST(StringVectorTest, InsertMoveTest) {
  StringVector vector;
  StringVector::Iterator<base::MoveOnlyInt> iterator =
      vector.Iterated<base::MoveOnlyInt>();
  auto it = vector.insert(iterator.begin(), base::MoveOnlyInt(0));
  it = vector.insert(it, base::MoveOnlyInt(1));
  iterator = vector.Iterated<base::MoveOnlyInt>();
  it = vector.insert(iterator.begin() + 1, base::MoveOnlyInt(2));
  it = vector.insert(it, base::MoveOnlyInt(3));
  vector.insert(vector.Iterated<base::MoveOnlyInt>().end(),
                base::MoveOnlyInt(4));
  vector.push_back(base::MoveOnlyInt(5));

  int answer[] = {1, 3, 2, 0, 4, 5};
  for (size_t i = 0; i < base::size(answer); ++i) {
    EXPECT_EQ(answer[i], vector.at<base::MoveOnlyInt>(i).data());
  }
}

TEST(StringVectorTest, EraseTest) {
  StringVector vector;
  for (int i = 0; i < 10; ++i) {
    vector.push_back<int>(i);
  }

  auto it = vector.Iterated<int>().begin();
  it = vector.erase<int>(it + 5);
  EXPECT_EQ(6, *it);
  EXPECT_EQ(9, vector.size<int>());

  it = vector.Iterated<int>().begin();
  it = vector.erase<int>(it + 5, it + 7);
  EXPECT_EQ(8, *it);
  EXPECT_EQ(7, vector.size<int>());
}

}  // namespace felicia
// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/containers/data.h"

#include "gtest/gtest.h"

#include "third_party/chromium/base/stl_util.h"

#include "felicia/core/lib/unit/geometry/point.h"

namespace felicia {

TEST(DataTest, PushAndPopTest) {
  Data vector;
  Data::View<Pointi> view = vector.AsView<Pointi>();
  Pointi p(1, 2);
  Pointi p2(3, 4);
  view.push_back(p);
  view.push_back(p2);
  EXPECT_EQ(view.size(), 2);

  Pointi& p3 = view[0];
  EXPECT_EQ(p, p3);
  EXPECT_EQ(p, view.front());
  Pointi& p4 = view[1];
  EXPECT_EQ(p2, p4);
  EXPECT_EQ(p2, view.back());

  p3.set_x(5);
  EXPECT_EQ(p3.x(), 5);
  EXPECT_EQ(view[0].x(), 5);

  view.pop_back();
  EXPECT_EQ(1, view.size());
  EXPECT_EQ(view.front(), view.back());
}

namespace {

template <typename Iterator>
void ExpectIteratorEq(Iterator begin, Iterator end, int from, int to) {
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

TEST(DataTest, IteratorTest) {
  Data vector;
  Data::View<int> view = vector.AsView<int>();
  for (int i = 0; i < 10; ++i) {
    view.push_back(i);
  }

  ExpectIteratorEq(view.begin(), view.end(), 0, 10);
  ExpectIteratorEq(view.cbegin(), view.cend(), 0, 10);
  ExpectIteratorEq(view.rbegin(), view.rend(), 9, -1);
  ExpectIteratorEq(view.crbegin(), view.crend(), 9, -1);

  Data::ConstView<int> cview = vector.AsConstView<int>();
  ExpectIteratorEq(cview.begin(), cview.end(), 0, 10);
  ExpectIteratorEq(cview.cbegin(), cview.cend(), 0, 10);
  ExpectIteratorEq(cview.rbegin(), cview.rend(), 9, -1);
  ExpectIteratorEq(cview.crbegin(), cview.crend(), 9, -1);
}

TEST(DataTest, InsertTest) {
  Data vector;
  Data::View<int> view = vector.AsView<int>();
  auto it = view.insert(view.begin(), 0);
  it = view.insert(it, 1);
  it = view.insert(view.begin() + 1, 2);
  it = view.insert(it, 3);
  view.insert(it, 3, 4);
  view.insert(view.end(), 5);
  view.push_back(6);

  int answer[] = {1, 4, 4, 4, 3, 2, 0, 5, 6};
  for (size_t i = 0; i < base::size(answer); ++i) {
    EXPECT_EQ(answer[i], view[i]);
  }
}

TEST(DataTest, EraseTest) {
  Data vector;
  Data::View<int> view = vector.AsView<int>();
  for (int i = 0; i < 10; ++i) {
    view.push_back(i);
  }

  auto it = view.erase(view.begin() + 5);
  EXPECT_EQ(6, *it);
  EXPECT_EQ(9, view.size());

  it = view.erase(view.begin() + 5, view.begin() + 7);
  EXPECT_EQ(8, *it);
  EXPECT_EQ(7, view.size());
}

}  // namespace felicia
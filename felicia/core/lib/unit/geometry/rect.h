// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_LIB_UNIT_GEOMETRY_RECT_H_
#define FELICIA_CORE_LIB_UNIT_GEOMETRY_RECT_H_

#include "third_party/chromium/base/strings/stringprintf.h"

#include "felicia/core/lib/base/export.h"
#include "felicia/core/lib/unit/geometry/point.h"
#include "felicia/core/lib/unit/geometry/size.h"

namespace felicia {

template <typename T>
class Rect {
 public:
  constexpr Rect() = default;
  constexpr Rect(const Point<T>& top_left, const Point<T>& bottom_right)
      : top_left_(top_left), bottom_right_(bottom_right) {
    CHECK(bottom_right.x() > top_left.x());
    CHECK(bottom_right.y() > top_left.y());
  }
  constexpr Rect(const Point<T>& top_left, T width, T height)
      : Rect(top_left, Point<T>{top_left.x() + width, top_left.y() + height}) {}

  constexpr Rect(const Rect& other) = default;
  Rect& operator=(const Rect& other) = default;

  constexpr const Point<T>& top_left() const { return top_left_; }
  constexpr const Point<T>& bottom_right() const { return bottom_right_; }
  constexpr T width() const { return bottom_right_.x() - top_left_.x(); }
  constexpr T height() const { return bottom_right_.y() - top_left_.y(); }
  constexpr Size<T> size() const { return {width(), height()}; }

  constexpr T area() const { return CheckedArea().ValueOrDie(); }

  std::string ToString() const {
    return base::StringPrintf("[%s, %s]", top_left_.ToString(),
                              bottom_right_.ToString());
  }

 private:
  constexpr base::CheckedNumeric<T> CheckedArea() const {
    base::CheckedNumeric<T> checked_area = width();
    checked_area *= height();
    return checked_area;
  }

  Point<T> top_left_;
  Point<T> bottom_right_;
};

template <typename T>
inline bool operator==(const Rect<T>& lhs, const Rect<T>& rhs) {
  return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

template <typename T>
inline bool operator!=(const Rect<T>& lhs, const Rect<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Rect<T>& size) {
  return os << size.ToString();
}

typedef Rect<int> Recti;
typedef Rect<float> Rectf;
typedef Rect<double> Rectd;

template <typename RectMessageType, typename PointMessageType, typename T>
RectMessageType RectToRectMessage(const Rect<T>& rect) {
  RectMessageType message;
  *message.mutable_top_left() =
      PointToPointMessage<PointMessageType>(rect.top_left());
  *message.mutable_bottom_right() =
      PointToPointMessage<PointMessageType>(rect.bottom_right());
  return message;
}

FEL_EXPORT RectiMessage RectiToRectiMessage(const Recti& rect);
FEL_EXPORT RectfMessage RectfToRectfMessage(const Rectf& rect);
FEL_EXPORT RectdMessage RectdToRectdMessage(const Rectd& rect);

template <typename T, typename MessageType>
Rect<T> RectMessageToRect(const MessageType& message) {
  return {PointMessageToPoint<T>(message.top_left()),
          PointMessageToPoint<T>(message.bottom_right())};
}

FEL_EXPORT Recti RectiMessageToRecti(const RectiMessage& message);
FEL_EXPORT Rectf RectfMessageToRectf(const RectfMessage& message);
FEL_EXPORT Rectd RectdMessageToRectd(const RectdMessage& message);

}  // namespace felicia

#endif  // FELICIA_CORE_LIB_UNIT_GEOMETRY_RECT_H_
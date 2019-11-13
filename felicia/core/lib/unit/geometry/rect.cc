// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/unit/geometry/rect.h"

namespace felicia {

RectiMessage RectiToRectiMessage(const Recti& rect) {
  return RectToRectMessage<RectiMessage, PointiMessage>(rect);
}

RectfMessage RectfToRectfMessage(const Rectf& rect) {
  return RectToRectMessage<RectfMessage, PointfMessage>(rect);
}

RectdMessage RectdToRectdMessage(const Rectd& rect) {
  return RectToRectMessage<RectdMessage, PointdMessage>(rect);
}

Recti RectiMessageToRecti(const RectiMessage& message) {
  return RectMessageToRect<int>(message);
}

Rectf RectfMessageToRectf(const RectfMessage& message) {
  return RectMessageToRect<float>(message);
}

Rectd RectdMessageToRectd(const RectdMessage& message) {
  return RectMessageToRect<double>(message);
}

}  // namespace felicia
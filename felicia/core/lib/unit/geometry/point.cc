// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/unit/geometry/point.h"

namespace felicia {

PointiMessage PointiToPointiMessage(const Pointi& point) {
  return PointToPointMessage<PointiMessage>(point);
}

PointfMessage PointfToPointfMessage(const Pointf& point) {
  return PointToPointMessage<PointfMessage>(point);
}

PointdMessage PointdToPointdMessage(const Pointd& point) {
  return PointToPointMessage<PointdMessage>(point);
}

Pointi PointiMessageToPointi(const PointiMessage& message) {
  return PointMessageToPoint<int>(message);
}

Pointf PointfMessageToPointf(const PointfMessage& message) {
  return PointMessageToPoint<float>(message);
}

Pointd PointdMessageToPointd(const PointdMessage& message) {
  return PointMessageToPoint<double>(message);
}

Point3iMessage Point3iToPoint3iMessage(const Point3i& point) {
  return Point3ToPoint3Message<Point3iMessage>(point);
}

Point3fMessage Point3fToPoint3fMessage(const Point3f& point) {
  return Point3ToPoint3Message<Point3fMessage>(point);
}

Point3dMessage Point3dToPoint3dMessage(const Point3d& point) {
  return Point3ToPoint3Message<Point3dMessage>(point);
}

Point3i Point3iMessageToPoint3i(const Point3iMessage& message) {
  return Point3MessageToPoint3<int>(message);
}

Point3f Point3fMessageToPoint3f(const Point3fMessage& message) {
  return Point3MessageToPoint3<float>(message);
}

Point3d Point3dMessageToPoint3d(const Point3dMessage& message) {
  return Point3MessageToPoint3<double>(message);
}

}  // namespace felicia
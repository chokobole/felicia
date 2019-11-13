// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/lib/unit/geometry/quaternion.h"

namespace felicia {

QuaternionfMessage QuaternionfToQuaternionfMessage(
    const Quaternionf& quaternion) {
  return QuaternionToQuaternionMessage<QuaternionfMessage>(quaternion);
}

QuaterniondMessage QuaterniondToQuaterniondMessage(
    const Quaterniond& quaternion) {
  return QuaternionToQuaternionMessage<QuaterniondMessage>(quaternion);
}

Quaternionf QuaternionfMessageToQuaternionf(const QuaternionfMessage& message) {
  return QuaternionMessageToQuaternion<float>(message);
}

Quaterniond QuaterniondMessageToQuaterniond(const QuaterniondMessage& message) {
  return QuaternionMessageToQuaternion<double>(message);
}

}  // namespace felicia
// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "felicia/core/master/bytes_constants.h"

namespace felicia {

Bytes kHeartBeatBytes = Bytes::FromBytes(128);
Bytes kMasterNotificationBytes = Bytes::FromBytes(256);

}  // namespace felicia
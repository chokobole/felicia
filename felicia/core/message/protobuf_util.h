// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FELICIA_CORE_MESSAGE_PROTOBUF_UTIL_H_
#define FELICIA_CORE_MESSAGE_PROTOBUF_UTIL_H_

#include "google/protobuf/message.h"

#include "felicia/core/lib/base/export.h"

namespace felicia {
namespace protobuf {

FEL_EXPORT std::string ProtobufMessageToString(
    const google::protobuf::Message& message);

}  // namespace protobuf
}  // namespace felicia

#endif  // FELICIA_CORE_MESSAGE_PROTOBUF_UTIL_H_